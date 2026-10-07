#include "blackbird/workflows.hpp"
#include "blackbird/tools.hpp"
#include <algorithm>
#include <mutex>
#include <set>
namespace blackbird {
namespace {
std::mutex mutex;
std::shared_ptr<const WorkflowRegistry> displayed;
bool word(unsigned char c) {
  return c >= 128 || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c == '_';
}
bool identifier(std::string_view s) {
  return !s.empty() && s.size() <= 64 && std::all_of(s.begin(), s.end(), [](char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
           c == '_' || c == '-';
  });
}
Ink ink(std::string_view s) {
  if (s == "keyword")
    return Ink::keyword;
  if (s == "success")
    return Ink::success;
  if (s == "heading")
    return Ink::heading;
  if (s == "failure")
    return Ink::failure;
  throw Error{ErrorCode::invalid_range};
}
const std::string &str(const Json &j, std::string_view k) {
  return field(j, k).string();
}
} // namespace
WorkflowRegistry::WorkflowRegistry(const Json &config, std::string revision) try
    : revision_(std::move(revision)) {
  if (const auto *p = config.find("workflow_prefix"))
    prefix_ = p->string();
  if (!prefix_.empty() && !identifier(prefix_))
    throw Error{ErrorCode::invalid_range};
  if (const auto *d = config.find("workflows"))
    definitions_ = *d;
  if (unwrap(dump_json(definitions_)).size() > 65536 ||
      definitions_.array().size() > 32)
    throw Error{ErrorCode::capacity};
  std::set<std::string> names, slash_aliases, bare_aliases, tokens;
  for (const auto &n : terminal_builtin_names()) {
    slash_aliases.insert(n.substr(1));
    bare_aliases.insert(n.substr(1));
  }
  std::size_t index = 0;
  for (const auto &d : definitions_.array()) {
    if (d.object().size() != 6 || !identifier(str(d, "name")) ||
        !names.insert(str(d, "name")).second || str(d, "description").empty() ||
        str(d, "description").size() > 256 ||
        std::any_of(str(d, "description").begin(), str(d, "description").end(),
                    [](char ch) {
                      const auto c = static_cast<unsigned char>(ch);
                      return c < 32 || c == 127;
                    }) ||
        str(d, "source").empty() || str(d, "source").size() > 16384)
      throw Error{ErrorCode::invalid_range};
    (void)std::get<bool>(field(d, "bare").value());
    const auto &as = field(d, "aliases").array();
    if (as.empty() || as.size() > 8)
      throw Error{ErrorCode::invalid_range};
    for (const auto &a : as) {
      if (!identifier(a.string()) || !slash_aliases.insert(prefix_ + a.string()).second)
        throw Error{ErrorCode::conflict};
      aliases_.emplace("/" + prefix_ + a.string(), index);
      if (std::get<bool>(field(d, "bare").value())) {
        if (!bare_aliases.insert(a.string()).second)
          throw Error{ErrorCode::conflict};
        aliases_.emplace(a.string(), index);
      }
    }
    const auto &ps = field(d, "powerwords").array();
    if (ps.size() > 8)
      throw Error{ErrorCode::capacity};
    for (const auto &p : ps) {
      const auto token = str(p, "token");
      if (p.object().size() != 2 || token.empty() || token.size() > 64 ||
          !std::all_of(token.begin(), token.end(),
                       [](char c) {
                         return static_cast<unsigned char>(c) < 128 &&
                                word(static_cast<unsigned char>(c));
                       }) ||
          !tokens.insert(token).second)
        throw Error{ErrorCode::conflict};
      powerwords_.emplace(token, Powerword{index, ink(str(p, "color"))});
    }
    ++index;
  }
} catch (const std::bad_variant_access &) {
  throw Error{ErrorCode::invalid_range};
}
Json WorkflowRegistry::discover() const {
  return Json::object({{"revision", Json{revision_}},
                       {"prefix", Json{prefix_}},
                       {"definitions", definitions_}});
}
const Json &WorkflowRegistry::named(std::string_view name) const {
  for (const auto &d : definitions_.array())
    if (str(d, "name") == name)
      return d;
  throw Error{ErrorCode::invalid_range};
}
std::optional<WorkflowSelection> WorkflowRegistry::select(std::string_view text) const {
  const auto end = text.find_first_of(" \t\r\n");
  const auto first = text.substr(0, end);
  if (const auto it = aliases_.find(first); it != aliases_.end())
    return WorkflowSelection{
        definitions_.array()[it->second],
        end == std::string_view::npos ? "" : std::string{text.substr(end)},
        std::string{first}};
  if (text.starts_with("/"))
    return {};
  std::optional<WorkflowSelection> selected;
  for (std::size_t b = 0; b < text.size();) {
    if (!word(static_cast<unsigned char>(text[b]))) {
      ++b;
      continue;
    }
    auto e = b + 1;
    while (e < text.size() && word(static_cast<unsigned char>(text[e])))
      ++e;
    if (const auto it = powerwords_.find(text.substr(b, e - b));
        it != powerwords_.end()) {
      const auto &d = definitions_.array()[it->second.definition];
      if (selected && str(selected->definition, "name") != str(d, "name"))
        throw Error{ErrorCode::conflict};
      if (!selected)
        selected = WorkflowSelection{d, std::string{text}, it->first};
    }
    b = e;
  }
  return selected;
}
ChatRow WorkflowRegistry::color(std::string_view text, Ink base) const {
  ChatRow out;
  std::size_t last = 0;
  for (std::size_t b = 0; b < text.size();) {
    if (!word(static_cast<unsigned char>(text[b]))) {
      ++b;
      continue;
    }
    auto e = b + 1;
    while (e < text.size() && word(static_cast<unsigned char>(text[e])))
      ++e;
    if (const auto it = powerwords_.find(text.substr(b, e - b));
        it != powerwords_.end()) {
      if (b > last)
        out.push_back({std::string{text.substr(last, b - last)}, base});
      out.push_back({std::string{text.substr(b, e - b)}, it->second.ink});
      last = e;
    }
    b = e;
  }
  if (last < text.size())
    out.push_back({std::string{text.substr(last)}, base});
  return out;
}
void publish_workflows(std::shared_ptr<const WorkflowRegistry> r) {
  std::lock_guard lock(mutex);
  displayed = std::move(r);
}
std::shared_ptr<const WorkflowRegistry> displayed_workflows() {
  std::lock_guard lock(mutex);
  return displayed;
}
Json default_workflows() {
  return Json{Json::Array{Json::object(
      {{"name", Json{"ultracode"}},
       {"description", Json{"Bounded source-grounded coding with direct checks"}},
       {"aliases", Json{Json::Array{Json{"ultracode"}}}},
       {"bare", Json{false}},
       {"powerwords", Json{Json::Array{Json::object({{"token", Json{"ultracode"}},
                                                     {"color", Json{"keyword"}}})}}},
       {"source",
        Json{
            R"lua(blackbird.append({{role='developer',content='Ultracode: implement useful working code. Read targeted sources, state concrete tasks and a fixed allowance before hardening. Preserve the operator prompt. Use direct specification oracles and fake effects where possible. One independent review and one findings recheck within the allowance; never reset it. Report actual outcomes and gaps; no invented scheduler or performance claims.'}})
for step=1,64 do
 local r=blackbird.request(); local calls=0
 for _,item in ipairs(r.output) do
  if item.type=='function_call' then
   calls=calls+1; local result=blackbird.call(item.name,item.arguments)
   blackbird.append({{type='function_call_output',call_id=item.call_id,output=blackbird.json.encode(result)}})
  elseif item.type=='message' then blackbird.present(item) end
 end
 if blackbird.restarting() or calls==0 then return end
end
error('ultracode reached its 64-step budget')
)lua"}}})}};
}
} // namespace blackbird

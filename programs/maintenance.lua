-- Named maintenance module: load retained source with blackbird.module("maintenance").
-- Effects occur only when inventory is called, through the ordinary native audit.
local M = {}
function M.inventory(paths, terms, max_lines)
  local result = {}
  for _, path in ipairs(paths) do
    local read = blackbird.call("read_file", {path=path, range={mode="lines", start=1, ["end"]=max_lines}})
    assert(read.content, "source read failed: " .. path)
    local matches, line = {}, 0
    for text in (read.content .. "\n"):gmatch("([^\n]*)\n") do
      line = line + 1
      for _, term in ipairs(terms) do
        if text:find(term, 1, true) then
          matches[#matches+1] = {line=line, term=term, text=text}
          break
        end
      end
    end
    result[#result+1] = {path=path, matches=blackbird.array(matches)}
  end
  return blackbird.array(result)
end
return M

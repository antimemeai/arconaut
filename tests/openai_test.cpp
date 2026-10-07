#include "blackbird/openai.hpp"
#include <iostream>
#include <stdexcept>
using namespace blackbird;
void check(bool good) {
  if (!good)
    throw std::runtime_error("OpenAI stream contract");
}
int main() {
  try {
    const auto stream = ": ping\r\n\r\nevent: response.output_text.delta\r\ndata: "
                        "{\"type\":\"response.output_text.delta\",\"delta\":\"Hi\"}"
                        "\r\n\r\nevent: response.completed\r\ndata: "
                        "{\"type\":\"response.completed\",\"response\":{\"status\":"
                        "\"completed\",\"output\":[{\"type\":\"reasoning\",\"encrypted_"
                        "content\":\"opaque==\"},{\"type\":\"function_call\",\"call_"
                        "id\":\"abc\",\"arguments\":\"{}\"}]}}\r\n\r\n";
    const std::string delta = "data: "
                              "{\"type\":\"response.output_text.delta\",\"item_id\":"
                              "\"m1\",\"delta\":\"hello\"}\r\n\r\n";
    for (std::size_t cut = 0; cut <= delta.size(); ++cut) {
      ResponsePreview preview;
      auto first = preview.feed(std::string_view{delta}.substr(0, cut));
      auto second = preview.feed(std::string_view{delta}.substr(cut));
      first.insert(first.end(), second.begin(), second.end());
      check(first.size() == 1 && first[0].item_id == "m1" && first[0].text == "hello");
    }
    ResponsePreview bytewise;
    std::size_t previews = 0;
    for (const auto c : delta)
      previews += bytewise.feed(std::string_view{&c, 1}).size();
    check(previews == 1);
    check(bytewise
              .feed("data: "
                    "{\"type\":\"response.function_call_arguments.delta\",\"delta\":"
                    "\"bad\"}\n\n")
              .empty());
    ResponsePreview unattributed;
    check(unattributed
              .feed("data: "
                    "{\"type\":\"response.output_text.delta\",\"delta\":\"no-id\"}\n\n")
              .empty());
    ResponsePreview multiline;
    const auto pieces = multiline.feed(
        "event: message\ndata: {\"type\":\"response.output_text.delta\",\n: "
        "keepalive\ndata: \"item_id\":\"m2\",\"delta\":\"two\"}\n\n");
    check(pieces.size() == 1 && pieces[0].text == "two");
    auto response = completed_response(stream);
    check(response.has_value());
    check(response.value()
              .find("output")
              ->array()[0]
              .find("encrypted_content")
              ->string() == "opaque==");
    check(response.value().find("output")->array()[1].find("call_id")->string() ==
          "abc");
    const std::string done =
        "data: "
        "{\"type\":\"response.output_item.done\",\"output_index\":0,\"item\":{\"type\":"
        "\"message\",\"content\":[{\"type\":\"output_text\",\"text\":\"Hi\"}]}}\n\n";
    const std::string end = "data: "
                            "{\"type\":\"response.completed\",\"response\":{\"status\":"
                            "\"completed\",\"output\":[]}}\n\n";
    auto sparse = completed_response(done + end);
    check(sparse.has_value());
    check(sparse.value()
              .find("output")
              ->array()[0]
              .find("content")
              ->array()[0]
              .find("text")
              ->string() == "Hi");
    check(!completed_response(done + done + end).has_value());
    check(!completed_response(end + done).has_value());
    check(!completed_response("data: "
                              "{\"type\":\"response.output_item.done\",\"output_"
                              "index\":1,\"item\":{}}\n\n" +
                              end)
               .has_value());
    for (const auto s :
         {"", "data: {}\n\n",
          "data: "
          "{\"type\":\"response.completed\",\"response\":{\"status\":\"failed\"}}\n\n",
          "data: {\"type\":\"response.failed\"}\n\n",
          "data: {\"type\":\"response.incomplete\"}\n\n", "data: {bad}\n\n"})
      check(!completed_response(s).has_value());
    check(!completed_response(std::string{stream} + "data: {\"type\":\"error\"}\n\n")
               .has_value());
    check(!completed_response(stream, {32, 64, 16}).has_value());
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}

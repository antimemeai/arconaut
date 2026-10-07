-- Named maintenance module: load retained source with arco.module("maintenance").
-- Effects occur only when inventory is called, through the ordinary native audit.
local M = {}
function M.inventory(paths, terms, max_lines)
  local result = {}
  for _, path in ipairs(paths) do
    local read = arco.call("read_file", {path=path, line_start=1, line_end=max_lines})
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
    result[#result+1] = {path=path, matches=arco.array(matches)}
  end
  return arco.array(result)
end
return M

-- Effective source is retained each turn. Edit this program; next turn reloads it.
for step = 1, 64 do
  local response = arco.request()
  local calls = 0
  for _, item in ipairs(response.output) do
    if item.type == "function_call" then
      calls = calls + 1
      local result = arco.call(item.name, item.arguments)
      arco.append({
        {
          type = "function_call_output",
          call_id = item.call_id,
          output = arco.json.encode(result),
        },
      })
    elseif item.type == "message" then
      arco.present(item)
    end
  end
  if arco.restarting() or calls == 0 then
    return
  end
end
error("Workflow reached its configured 64-step budget; context is retained.")

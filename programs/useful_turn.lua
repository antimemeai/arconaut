-- Model-governed useful-work workflow. Configure paths normally in source.
-- Both source files and boundary steer are retained through audited reads.
local source=arco.call('read_file',{path='programs/useful_work.lua'}).content
work=assert(load(source,'useful-work-helpers','t'))()
local boundary=work.boundary('context/useful-work/steer.json')
arco.append({{role='assistant',content={{type='output_text',text=
  'Useful-work boundary: '..arco.json.encode(boundary)}}}})
for step=1,64 do
  local response=arco.request()
  local calls=0
  for _,item in ipairs(response.output) do
    if item.type=='function_call' then
      calls=calls+1
      local result=arco.call(item.name,item.arguments)
      arco.append({{type='function_call_output',call_id=item.call_id,output=arco.json.encode(result)}})
    elseif item.type=='message' then arco.present(item) end
  end
  if arco.restarting() or calls==0 then return end
end
error('Useful-work 64-step budget exhausted; retained work, no automatic replay.')

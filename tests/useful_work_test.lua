local writes,executions,requests=0,0,{}
local watermark={"130"}
blackbird={json={encode=function() return '{}' end,decode=function(v) return {direction=v} end},
  stats=function() return {entries={{id='large',item_bytes={'800'}},{id='small',item_bytes={'10'}}},
    input_bytes={'900'},last_request_bytes={'1000'},usage='unavailable'} end,
  call=function(name,args)
    if name=='audit_inspect' then
      requests[#requests+1]=args.query
      local cursor=type(args.query.cursor)=='table' and tonumber(args.query.cursor[1]) or args.query.cursor
      cursor=cursor or 0
      local next_cursor=math.min(cursor+64,130)
      return {['end']=watermark,next={tostring(next_cursor)},records={{label='lua.error',record={tostring(cursor)}}}}
    elseif name=='write_file' then writes=writes+1; return {written=true}
    elseif name=='exec' then executions=executions+1; return {timed_out=true,effect_outcome='unknown'}
    elseif name=='read_file' then return {content='next boundary steer'} end
    error('unexpected call')
  end}
local M=assert(loadfile(helper_path))()
assert(M.number({'14564'})==14564)
assert(not pcall(M.number,nil),'missing must not become zero')
local p=M.page(0,32,watermark)
assert(requests[1]['end']==watermark,'lossless tagged watermark')
local selected=M.find(0,watermark,1,function(row) return row.label=='lua.error' end)
assert(not selected.complete and M.number(selected.next)==64 and #selected.matches==1,'bounded nonexhaustive')
local rest=M.find(selected.next,selected.watermark,2,function(row) return row.label=='lua.error' end)
assert(rest.complete and #rest.matches==2,'separate occurrences retained')
local f=M.feedback()
assert(f.largest_entries[1].id=='large' and f.usage=='unavailable','actual feedback, unavailable not zero')
local b=M.boundary('steer')
assert(b.steer.direction=='next boundary steer')
assert(not pcall(M.contrast,'pool','request',{}),'missing independent discriminator refused')
local r=M.contrast('pool','request',{hypothesis='actual parser trace',discriminator='same original source bytes',
  discriminator_declared_before_attempt=true,baseline='failed reserved end',candidate='positional helper',
  measurement={correct=true},disposition='keep',references={record=15023}})
assert(r.effect_outcome=='unknown' and executions==1 and writes==1,'no unknown delivery replay')
print('useful Lua fault checks passed')
-- A failed request write must not execute a stale/missing request file.
local prior=blackbird.call
blackbird.call=function(name,args)
  if name=='write_file' then return {error='parent absent'} end
  return prior(name,args)
end
local before=executions
local rejected=M.record('pool','absent/request',{question='write failure',action='persist',outcome='unknown',references={}})
assert(executions==before and rejected.error,'failed write must stop CLI dispatch')
local ranges={}
blackbird.call=function(name,args)
 assert(name=='read_file');ranges[#ranges+1]=args;return {content='exact range'}
end
assert(M.lines('file',{'2'},4).content=='exact range')
assert(ranges[1].range.mode=='lines' and ranges[1].range.start==2 and ranges[1].range['end']==4 and ranges[1].line_start==nil and ranges[1].byte_start==nil)
assert(M.bytes('file',0,10).content=='exact range')
assert(ranges[2].range.mode=='bytes' and ranges[2].range.start==0 and ranges[2].range['end']==10 and ranges[2].byte_start==nil and ranges[2].line_start==nil)
assert(not pcall(M.lines,'file',0,2) and not pcall(M.bytes,'file',2,1))
assert(not pcall(M.lines,'file',1.5,2) and not pcall(M.bytes,'file',nil,1))

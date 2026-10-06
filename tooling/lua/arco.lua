---@meta
---@class ArcoContextEntry
---@field id string
---@field item table
---@class ArcoContext
---@field base string
---@field entries ArcoContextEntry[]
---@class ArcoEditOutcome
---@field accepted boolean
---@field revision string
---@field current string
---@field reason string
arco = {}
---@param options? table Request assembly overrides, including model/tools/input/reasoning.
---@return table response
function arco.request(options) end
---@param name string
---@param arguments table|string JSON arguments or decoded object.
---@return any result
function arco.call(name, arguments) end
---@return ArcoContext
function arco.context() end
---@param candidate ArcoContext
---@return ArcoEditOutcome
function arco.edit(candidate) end
---@return ArcoContextEntry[]
function arco.originals() end
---@param id string
---@return ArcoContext
function arco.restore(id) end
---@param items table[]
---@return ArcoContext
function arco.append(items) end
---@param text string
function arco.display(text) end
---@generic T: table
---@param value T
---@return T
function arco.array(value) end
arco.json = {}
---@param text string
---@return any
function arco.json.decode(text) end
---@param value any
---@return string
function arco.json.encode(value) end

---@param item table Completed assistant message; avoids duplicate preview.
function arco.present(item) end

---@return boolean Restart scheduled for successful turn boundary.
function arco.restarting() end

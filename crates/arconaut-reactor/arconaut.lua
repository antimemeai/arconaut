-- arconaut.lua — bridge between arconaut-reactor and neovim
-- Loaded on NvimRuntime::spawn()

-- Use _G instead of vim.g for function storage because vim.g's metatable
-- copies tables on access, preventing nested mutations from persisting.
local _arconaut = {}

-- Run a tree-sitter query on the current buffer.
-- Returns a list of { type, name, range, text } tables.
function _arconaut.ArconautQueryAST(query_string)
    local bufnr = vim.api.nvim_get_current_buf()
    local lang = vim.treesitter.language.get_lang(vim.bo[bufnr].filetype)
    if not lang then
        return {}
    end

    local parser = vim.treesitter.get_parser(bufnr, lang)
    if not parser then
        return {}
    end

    local tree = parser:parse()[1]
    if not tree then
        return {}
    end

    local root = tree:root()
    local query = vim.treesitter.query.parse(lang, query_string)
    if not query then
        return {}
    end

    local results = {}
    for id, node, _ in query:iter_captures(root, bufnr, 0, -1) do
        local name = query.captures[id]
        local text = vim.treesitter.get_node_text(node, bufnr)
        local range = { node:range() }
        table.insert(results, {
            capture = name,
            type = node:type(),
            range = {
                start_row = range[1],
                start_col = range[2],
                end_row = range[3],
                end_col = range[4],
            },
            text = text,
        })
    end

    return results
end

-- Replace the text of the first match for a tree-sitter query.
-- Returns { replaced = true/false, capture = name, range = {...} }.
function _arconaut.ArconautEditByQuery(query_string, replacement)
    local bufnr = vim.api.nvim_get_current_buf()
    local lang = vim.treesitter.language.get_lang(vim.bo[bufnr].filetype)
    if not lang then
        return { replaced = false, reason = "no language detected" }
    end

    local parser = vim.treesitter.get_parser(bufnr, lang)
    if not parser then
        return { replaced = false, reason = "no parser" }
    end

    local tree = parser:parse()[1]
    if not tree then
        return { replaced = false, reason = "no tree" }
    end

    local root = tree:root()
    local query = vim.treesitter.query.parse(lang, query_string)
    if not query then
        return { replaced = false, reason = "query parse failed" }
    end

    for id, node, _ in query:iter_captures(root, bufnr, 0, -1) do
        local name = query.captures[id]
        local range = { node:range() }
        local start_row = range[1]
        local start_col = range[2]
        local end_row = range[3]
        local end_col = range[4]

        -- For single-line nodes, use nvim_buf_set_text.
        -- For multi-line, use nvim_buf_set_lines on the range.
        if start_row == end_row then
            vim.api.nvim_buf_set_text(bufnr, start_row, start_col, end_row, end_col, { replacement })
        else
            local lines = vim.split(replacement, "\n", { plain = true })
            vim.api.nvim_buf_set_lines(bufnr, start_row, end_row + 1, false, lines)
        end

        return {
            replaced = true,
            capture = name,
            range = {
                start_row = start_row,
                start_col = start_col,
                end_row = end_row,
                end_col = end_col,
            },
        }
    end

    return { replaced = false, reason = "no match" }
end

_G._arconaut = _arconaut

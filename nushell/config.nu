$env.config.show_banner = false
$env.config.buffer_editor = "nvim"
$env.config.table.header_on_separator = true


def simplify-path [p: path] {
    (do -i { $p | path relative-to $nu.home-path }) | match $in {
        null => $p
        '' => '~'
        $relative_pwd => ([~ $relative_pwd] | path join)
    }
}


# prompt
let colors = [
    [   index            admin                     non-admin         ];
    [    pwd,    (ansi { bg: '#283d50' }),  (ansi { bg: '#463d50' }) ],
    [ indicator, (ansi { bg: '#325078' }),  (ansi { bg: '#645078' }) ],
]

$env.PROMPT_COMMAND = {||
    let c = if (is-admin) { $colors.non-admin } else { $colors.admin }
    let dir = simplify-path $env.PWD | path split | last 3 | path join
    $"(ansi white)($c.0) ($dir) (ansi white)($c.1) "
}
$env.PROMPT_INDICATOR = $"> (ansi reset) "

# aliases
alias l = ls
source ../aliases/generated.nu

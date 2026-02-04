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


def lighten1 [] : float -> float {
    const m = (14.0 - 6.0) / (11.0 - 5.0)
    const y0 = $m * (-5.0 / 15.0) + 6.0 / 15.0
    $m * $in + $y0 | [$in 0.0] | math max | [$in 1.0] | math min
}

def lighten [] : list<float> -> list<float> {
    $in | each { |x| $x | lighten1 }
}

def color [ r: int, g: int, b: int ] : nothing -> list<float> {
    [ ($r / 255.0) ($g / 255.0) ($b / 255.0) ]
}

def "into hex" [] : int -> string {
    $in | into binary | encode hex | str substring 0..1
}

def "color str" [ colors: list<float> ] : nothing -> string {
    $"#($colors.0 | $in * 255 | math floor | into hex)($colors.1 | $in * 255 | math floor | into hex)($colors.2 | $in * 255 | math floor | into hex)"
}


# prompt
let colors = {
    admin:      (color 0x42 0x7d 0xdb)
    non-admin:  (color 0x32 0x50 0x78)
}
def generate-colors [ admin: list<float>, non_admin: list<float> ] : nothing -> table {
    [
        [   index                      admin                                       non-admin                           ];
        [    pwd     (ansi { bg: (color str $admin)             })   (ansi { bg: (color str $non_admin)             }) ]
        [ indicator  (ansi { bg: (color str ($admin | lighten)) })   (ansi { bg: (color str ($non_admin | lighten)) }) ]
    ]
}
let colors = generate-colors $colors.admin $colors.non-admin

$env.PROMPT_COMMAND = {||
    let c = if (is-admin) { $colors.non-admin } else { $colors.admin }
    let dir = simplify-path $env.PWD | path split | last 3 | path join
    $"(ansi white)($c.0) ($dir) ($c.1) "
}
$env.PROMPT_INDICATOR = $"(ansi white)> (ansi reset) "

# aliases
alias l = ls
alias la = ls -a
alias ll = ls -l
source ../aliases/generated.nu

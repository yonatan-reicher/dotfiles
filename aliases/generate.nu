let files = [
    'all.sh',
]
let generated = 'generated.nu'


def parse-sh-aliases [] : list<string> -> list<string> {
    $in
    # Filter comments and blank lines
    | where { $in | str starts-with "#" | not $in }
    | where { $in | str trim | $in != "" }
    | parse "alias {name}='{expr}'"
    | each { $"alias ($in.name) = ($in.expr)" }
}


$files
| each --flatten {|file| open $file | lines}
| parse-sh-aliases
| save --force $generated

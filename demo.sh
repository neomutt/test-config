# bash auto-completion for 'demo'

_demo_complete()
{
    local cur
    _get_comp_words_by_ref cur

    COMPREPLY=( $( compgen -W 'account address bool deep dump enum inherit initial long mbtable number quad regex set slist sort string synonym' -- "$cur" ) )
}

complete -F _demo_complete demo


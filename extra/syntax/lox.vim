if exists("b:current_syntax")
  finish
endif

syntax keyword loxConditional if else 
syntax keyword loxRepeat      for while
syntax keyword loxOperator    + - / * 
syntax keyword loxKeyword     class super fun return var print this
syntax keyword loxFunction    print
syntax keyword loxBoolean     true false
syntax keyword loxType        var nil
syntax match   loxNumber      "\d\+\.\d*" 
syntax match   loxComment     "%s/\/\*\(\_.*\)\*\//\1/"
syntax keyword loxTodo contained todo fixme revisit bug

highlight def link loxConditional Conditional
highlight def link loxRepeat      Repeat
highlight def link loxOperator    Operator
highlight def link loxKeyword     Keyword
highlight def link loxFunction    Function
highlight def link loxBoolean     Boolean
highlight def link loxType        Type
highlight def link loxNumber      Number
highlight def link loxComment     Comment 
highlight def link loxTodo        Todo

let b:current_syntax = "lox"


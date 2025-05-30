lexer grammar RiddleLexer;

@lexer::members {
    /** 判断前一个可见 token 是否允许在行尾插入分号 */
    bool shouldImplicitSemi() {
        // 上一个已 emit 的 token
        antlr4::Token *prev = _lastToken;
        if (prev == nullptr) return false;

        switch (prev->getType()) {
            case RiddleLexer::Identifier:
            case RiddleLexer::Decimal:
            case RiddleLexer::STRING:
            case RiddleLexer::CHAR:
            case RiddleLexer::Semi:
            case RiddleLexer::Return:
            case RiddleLexer::RightParen:
            case RiddleLexer::RightBracket:
            case RiddleLexer::RightCurly:
                return true;
            case Star:
                return isPointerTail();
            default:
                return false;
        }
    }

    /** 记录最后一个非隐藏信道 token，用来做上面的判断 */
    antlr4::Token *_lastToken = nullptr;  // 当前行最后一个
    antlr4::Token *_prevToken  = nullptr; // 倒数第二个

    antlr4::Token *emit() override {
        antlr4::Token *t = Lexer::emit();
        if (t->getChannel() == antlr4::Token::DEFAULT_CHANNEL) {
            _prevToken = _lastToken;
            _lastToken = t;
        }
        return t;
    }

    bool isPointerTail() {
        if (!_prevToken) return false;

        switch (_prevToken->getType()) {
            case RiddleLexer::Star:          // int**
            case RiddleLexer::RightParen:    // (T*)*
            case RiddleLexer::RightBracket:  // arr*
            case RiddleLexer::RightCurly:    // {...}*
                return true;
            default:
                return false;                // 其它情况认为是跨行表达式，不插 ;
        }
    }
}

//关键字
Var:'var';  //可变变量
Val:'val';  //不可变变量
For:'for';
While:'while';
Continue:'continue';
Break:'break';
If:'if';
Else:'else';
Func:'fun'; //函数定义
Return: 'return' ;
Import: 'import' ;
Package: 'package';
Class:  'class';
True:'true';
False:'false';
Null:'null';
Try:'try';
Catch:'catch';
Extern: 'extern';
// 修饰符
Override: 'override';
Static:'static';
Const:'const';
Public: 'public';
Protected: 'protected';
Private: 'private';
Virtual: 'virtual';
Operator: 'operator';
//可见字符
//基本运算符
Semi
    : ';'
    | { shouldImplicitSemi() }? Endl
    ;

LeftParen : '(' ;
RightParen : ')' ;
LeftBracket : '[' ;
RightBracket : ']' ;
LeftCurly : '{' ;
RightCurly : '}' ;
Colon:          ':';
Comma:          ',';
Equal:          '==';
NotEqual:       '!=';
Assign:         '=';
Greater:        '>';
GreaterEqual:   '>=';
Less:           '<';
LessEqual:      '<=';
LeftShift:      '<<';
RightShift:     '>>';
Add:            '+';
Sub:            '-';
Star:           '*';
Div:            '/';
Mod:            '%';
Not:            '!';
And:            '&';
AndAnd:         '&&';
Or:             '|';
OrOr:           '||';
Xor:            '^';
Dot:            '.';
DoubleQuotes:   '"';
Quotes:         '\'';
Tilde:          '~';
AddAssign:      '+=';
SubAssign:      '-=';
MulAssign:      '*=';
DivAssign:      '/=';
ModAssign:      '%=';
LeftShiftAssign: '<<=';
RightShiftAssign: '>>=';
AndAssign:      '&=';
OrAssign:       '|=';
XorAssign:      '^=';


Endl
  : '\r'? '\n' -> channel(HIDDEN)
  ;
//标识符
Identifier: [A-Za-z_] [A-Za-z_0-9]*;
//十六进制
Hexadecimal: '0x' [1-9A-F] HEX_DIGIT*;
//十进制
Decimal: '0' | [1-9] DIGIT*;
//八进制
Octal: '0' [1-7] OCTAL_DIGIT*;
//二进制
Binary: '0b' '1' BINARY_DIGIT*;
Float: Decimal Dot IntegerSequence;

IntegerSequence: DIGIT+;
HEX_DIGIT: [0-9A-F];
OCTAL_DIGIT: [0-7];
BINARY_DIGIT: [0-1];
DIGIT: [0-9];
STRING
    : '"' (ESC | ~["\\\r\n])* '"'
    ;

CHAR
    : '\'' (ESC | ~["\\\r\n]) '\''
    ;

fragment ESC
    : '\\' (["\\/] | [A-Za-z])
    ;

LINE_COMMENT : '//' ~[\r\n]* -> channel(HIDDEN) ;
BLOCK_COMMENT : '/*' .*? '*/' -> channel(HIDDEN) ;
WHITESPACE : [ \t]+ -> channel(HIDDEN) ;
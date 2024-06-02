#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cctype>

enum TokenType {
    NUMBER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    explicit Lexer(const std::string &input) : input(input), pos(0) {}

    Token getNextToken() {
        while (pos < input.length() && isspace(input[pos])) pos++;
        if (pos == input.length()) return {END, ""};

        char current_char = input[pos];

        if (isdigit(current_char)) {
            std::string num;
            while (pos < input.length() && isdigit(input[pos])) {
                num += input[pos++];
            }
            return {NUMBER, num};
        }

        if (current_char == '+') return advanceAndReturn(PLUS, "+");
        if (current_char == '-') return advanceAndReturn(MINUS, "-");
        if (current_char == '*') return advanceAndReturn(MUL, "*");
        if (current_char == '/') return advanceAndReturn(DIV, "/");
        if (current_char == '(') return advanceAndReturn(LPAREN, "(");
        if (current_char == ')') return advanceAndReturn(RPAREN, ")");

        throw std::runtime_error("Invalid character");
    }

private:
    Token advanceAndReturn(TokenType type, const std::string &value) {
        pos++;
        return {type, value};
    }

    std::string input;
    size_t pos;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print() const = 0;
    virtual int evaluate() const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

class NumberNode : public ASTNode {
public:
    explicit NumberNode(int value) : value(value) {}
    void print() const override { std::cout << value; }
    int evaluate() const override { return value; }
private:
    int value;
};

class OperatorNode : public ASTNode {
public:
    OperatorNode(TokenType op, ASTNodePtr left, ASTNodePtr right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    void print() const override {
        std::cout << '(';
        left->print();
        std::cout << ' ' << getOpChar(op) << ' ';
        right->print();
        std::cout << ')';
    }

    int evaluate() const override {
        int leftVal = left->evaluate();
        int rightVal = right->evaluate();
        switch (op) {
            case PLUS: return leftVal + rightVal;
            case MINUS: return leftVal - rightVal;
            case MUL: return leftVal * rightVal;
            case DIV: return leftVal / rightVal;
            default: throw std::runtime_error("Invalid operator");
        }
    }

private:
    TokenType op;
    ASTNodePtr left, right;

    static char getOpChar(TokenType op) {
        switch (op) {
            case PLUS: return '+';
            case MINUS: return '-';
            case MUL: return '*';
            case DIV: return '/';
            default: throw std::runtime_error("Invalid operator");
        }
    }
};

class Parser {
public:
    explicit Parser(Lexer lexer) : lexer(std::move(lexer)) {
        currentToken = this->lexer.getNextToken();
    }

    ASTNodePtr parse() {
        return parseExpression();
    }

private:
    Token currentToken;
    Lexer lexer;

    void consume(TokenType type) {
        if (currentToken.type == type) {
            currentToken = lexer.getNextToken();
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }

    ASTNodePtr parseExpression() {
        auto node = parseTerm();
        while (currentToken.type == PLUS || currentToken.type == MINUS) {
            TokenType op = currentToken.type;
            consume(op);
            node = std::make_unique<OperatorNode>(op, std::move(node), parseTerm());
        }
        return node;
    }

    ASTNodePtr parseTerm() {
        auto node = parseFactor();
        while (currentToken.type == MUL || currentToken.type == DIV) {
            TokenType op = currentToken.type;
            consume(op);
            node = std::make_unique<OperatorNode>(op, std::move(node), parseFactor());
        }
        return node;
    }

    ASTNodePtr parseFactor() {
        if (currentToken.type == NUMBER) {
            auto node = std::make_unique<NumberNode>(std::stoi(currentToken.value));
            consume(NUMBER);
            return node;
        } else if (currentToken.type == LPAREN) {
            consume(LPAREN);
            auto node = parseExpression();
            consume(RPAREN);
            return node;
        } else {
            throw std::runtime_error("Unexpected token in factor");
        }
    }
};

int main() {
    std::string input = "3 + 5 * (2 - 8)";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        ASTNodePtr ast = parser.parse();
        ast->print();
        std::cout << std::endl << "Result: " << ast->evaluate() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

#pragma once

#ifdef WIN32
#define x_stricmp _stricmp
#else
#define x_stricmp strcasecmp
#endif

namespace data { ;

struct token_t {
public:
    enum token_type
    {
        NONE, OPERATOR, CONSTANT, NUMBER
    };
    enum operator_type
    {
        NOOP, PLUS, MINUS, MULTIPLIER, DIVIDER, OPEN_PARENTHESIS, CLOSE_PARENTHESIS
    };

public:
    token_type type;
    operator_type op;
    std::string name;
    double value;

public:
    token_t()
        : type(NONE), op(NOOP), value(0) {}

    token_t(operator_type _op)
        : type(OPERATOR), op(_op), value(0) {}

    token_t(std::string _name)
        : type(CONSTANT), op(NOOP), name(_name), value(constant_map(_name)) {}

    token_t(double _value)
        : type(NUMBER), op(NOOP), value(_value) {}

public:
    static double constant_map(const std::string& name);
    static bool is_numeric(char ch);
    static bool is_operator(char ch);
    static token_t::operator_type to_operator_type(char ch);
};

inline double token_t::constant_map(const std::string& name)
{
    static struct constant_pair_t {
        const char* name;
        double value;

        constant_pair_t(const char* _name, double _value)
            : name(_name), value(_value) {}
    } name_map[] = {
        constant_pair_t("M_PI", 3.141592653589793238)
    };

    for (size_t index = 0; index < _countof(name_map); ++index) {
        if (x_stricmp(name_map[index].name, name.c_str()) == 0)
            return name_map[index].value;
    }
    return 0;
}

inline bool token_t::is_numeric(char ch)
{
    return std::string("0123456789.f").find(ch) != std::string().npos;
}

inline bool token_t::is_operator(char ch)
{
    return std::string("+-*/()").find(ch) != std::string().npos;
}

inline token_t::operator_type token_t::to_operator_type(char ch)
{
    switch (ch)
    {
    case '+': return token_t::PLUS;
    case '-': return token_t::MINUS;
    case '*': return token_t::MULTIPLIER;
    case '/': return token_t::DIVIDER;
    case '(': return token_t::OPEN_PARENTHESIS;
    case ')': return token_t::CLOSE_PARENTHESIS;
    }
    return token_t::NOOP;
}


class token_set_t {
public:
    typedef std::vector<token_t> token_list_t;
    typedef token_list_t::const_iterator token_iterator_t;

    void push(token_t::token_type new_type, char ch);
    void finalize(token_t::token_type new_type = token_t::NONE);

    token_iterator_t begin() const { return tokens.cbegin(); }
    token_iterator_t end() const { return tokens.cend(); }

    token_set_t()
        : type(token_t::NONE) {}
        
private:
    token_list_t tokens;
    token_t::token_type type;
    std::string lexim;
};

inline void token_set_t::push(token_t::token_type new_type, char ch)
{
    finalize(new_type);
    if (new_type == token_t::OPERATOR)
    {
        tokens.push_back(token_t(token_t::to_operator_type(ch)));
        type = token_t::NONE;
    }
    else
    {
        lexim.push_back(ch);
        type = new_type;
    }
}

inline void token_set_t::finalize(token_t::token_type new_type)
{
    if (type != token_t::NONE && type != new_type)
    {
        switch (type)
        {
        case token_t::CONSTANT:
            tokens.push_back(token_t(lexim));
            break;

        case token_t::NUMBER:
            tokens.push_back(token_t(atof(lexim.c_str())));
            break;
        }
        lexim.clear();
    }
    if (new_type == token_t::NONE)
        tokens.push_back(token_t());
}


class calculator_t {
public:
    double result();

    calculator_t(const token_set_t& token_set)
        : current(token_set.begin()) {}

private:
    double calc_expression();
    double calc_term();
    double calc_factor();

private:
    token_set_t::token_iterator_t current;
};

inline double calculator_t::result()
{
    return calc_expression();
}

inline double calculator_t::calc_expression()
{
    double lhs = calc_term();
    while (current->type == token_t::OPERATOR &&
        (current->op == token_t::PLUS || current->op == token_t::MINUS)) {
        token_t::operator_type op = current->op;
        ++current;

        double rhs = calc_term();
        lhs += (op == token_t::PLUS? 1: -1) * rhs;
    }
    return lhs;
}

inline double calculator_t::calc_term()
{
    double lhs = calc_factor();
    while (current->type == token_t::OPERATOR &&
        (current->op == token_t::MULTIPLIER || current->op == token_t::DIVIDER)) {
        token_t::operator_type op = current->op;
        ++current;

        double rhs = calc_factor();
        lhs *= (op == token_t::MULTIPLIER? rhs: 1.0 / rhs);
    }
    return lhs;
}

inline double calculator_t::calc_factor( )
{
    double result = 0;
    if (current->type == token_t::CONSTANT || current->type == token_t::NUMBER)
        result = current->value;

    if (current->type == token_t::OPERATOR && current->op == token_t::OPEN_PARENTHESIS)
    {
        ++current;
        result = calc_expression();
        assert(current->type == token_t::OPERATOR && current->op == token_t::CLOSE_PARENTHESIS);
    }
    ++current;
    return result;
}

class expression_parser_t {
public:
    double result() const;

    expression_parser_t(const char* _expression)
        : expression(_expression == NULL? "": _expression) {}

private:
    std::string expression;
};

inline double expression_parser_t::result() const
{
    if (expression.empty())
        return 0;

    token_set_t token_set;
    std::for_each(expression.begin(), expression.end(), [&token_set] (char ch) {
        if (ch == ' ' || ch == '\t') {
            return;

        } else if (token_t::is_operator(ch)) {
            token_set.push(token_t::OPERATOR, ch);

        } else if (token_t::is_numeric(ch)) {
            token_set.push(token_t::NUMBER, ch);
            
        } else {
            token_set.push(token_t::CONSTANT, ch);
        }
    });
    token_set.finalize();

    return calculator_t(token_set).result();
}

}

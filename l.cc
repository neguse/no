#include <cstdio>
#include <cassert>
#include <memory>
#include <map>
#include <vector>

struct Position {
  int line, col;
};

class Input {
private:
  FILE *fp_;
  Position pos_;

public:
  Input(const char *f) {
    fp_ = fopen(f, "r");
    pos_ = Position{1, 0};
  }

  char Get() {
    char c = fgetc(fp_);
    if (c == '\n') {
      pos_.line++;
      pos_.col = 0;
    } else if (c == '\t') {
      pos_.col += 4;
    } else if (c == EOF) {
    } else {
      pos_.col++;
    }
    return c;
  }

  Position pos() const { return pos_; }

  bool ok() const { return fp_ != nullptr; }
};

struct Node {
  typedef std::shared_ptr<Node> Ref;
  char c;
  Ref a;
  Ref d;
  Position pos;
};

enum class HierarchicalType {
  Leaf,
  Parent,
};

enum class SyntaxType {
  None,
  Program,
  ProgramCont,
  ProgramEnd,
  AssignExp,
  CallExp,
  LambdaExp,
  CallArgList,
  CallArgListCont,
  CallArgListEnd,
  CallArgEmpty,
  LambdaArgList,
  LambdaArgListCont,
  LambdaArgListEnd,
  OpExp,
  IfExp,
  IfBranch,
  Hex2,
  Hex,
  Variable,
};

typedef std::vector<SyntaxType> AcceptableSyntax;
struct Syntax {
  HierarchicalType ht;
  SyntaxType st;
  AcceptableSyntax aa;
  AcceptableSyntax ad;
};

class Lang {
private:
  std::map<char, Syntax> syntax_;

public:
  Lang() {
    auto nullAcceptable = AcceptableSyntax{};
    auto expAcceptable = AcceptableSyntax{
        SyntaxType::AssignExp, SyntaxType::CallExp,  SyntaxType::LambdaExp,
        SyntaxType::OpExp,     SyntaxType::IfExp,    SyntaxType::Hex2,
        SyntaxType::Hex,       SyntaxType::Variable,
    };

    for (char c = 'g'; c <= 'z'; c++) {
      syntax_[c] = Syntax{
          HierarchicalType::Leaf, SyntaxType::Variable, nullAcceptable,
          nullAcceptable,
      };
    }
    for (char c = 'A'; c <= 'Z'; c++) {
      syntax_[c] = Syntax{
          HierarchicalType::Leaf, SyntaxType::Variable, nullAcceptable,
          nullAcceptable,
      };
    }
    for (char c = 'a'; c <= 'f'; c++) {
      syntax_[c] = Syntax{
          HierarchicalType::Leaf, SyntaxType::Hex, nullAcceptable,
          nullAcceptable,
      };
    }
    for (char c = '0'; c <= '9'; c++) {
      syntax_[c] = Syntax{
          HierarchicalType::Leaf, SyntaxType::Hex, nullAcceptable,
          nullAcceptable,
      };
    }

    auto programContAccept =
        AcceptableSyntax{SyntaxType::ProgramCont, SyntaxType::ProgramEnd};
    {
      syntax_['{'] = Syntax{
          HierarchicalType::Parent, SyntaxType::Program, expAcceptable,
          programContAccept,

      };
    }
    {
      syntax_[';'] = Syntax{
          HierarchicalType::Parent, SyntaxType::ProgramCont, expAcceptable,
          programContAccept,
      };
    }
    {
      syntax_['}'] = Syntax{
          HierarchicalType::Leaf, SyntaxType::ProgramEnd, nullAcceptable,
          nullAcceptable,
      };
    }
    {
      auto hexAcceptable = AcceptableSyntax{SyntaxType::Hex};
      syntax_['#'] = Syntax{
          HierarchicalType::Parent, SyntaxType::Hex2, hexAcceptable,
          hexAcceptable,
      };
    }
    {
      syntax_['~'] = Syntax{
          HierarchicalType::Parent, SyntaxType::CallExp, expAcceptable,
          AcceptableSyntax{SyntaxType::CallArgList, SyntaxType::CallArgEmpty},
      };
    }
    {
      syntax_['('] = Syntax{
          HierarchicalType::Parent, SyntaxType::CallArgList, expAcceptable,
          AcceptableSyntax{SyntaxType::CallArgListCont,
                           SyntaxType::CallArgListEnd},
      };
    }
    {
      syntax_[','] = Syntax{
          HierarchicalType::Parent, SyntaxType::CallArgListCont, expAcceptable,
          AcceptableSyntax{SyntaxType::CallArgListCont,
                           SyntaxType::CallArgListEnd},
      };
    }
    {
      syntax_[')'] = Syntax{
          HierarchicalType::Leaf, SyntaxType::CallArgListEnd, nullAcceptable,
          nullAcceptable,
      };
    }
    {
      syntax_['_'] = Syntax{
          HierarchicalType::Leaf, SyntaxType::CallArgEmpty, nullAcceptable,
          nullAcceptable,
      };
    }
    {
      syntax_['\\'] = Syntax{
          HierarchicalType::Parent, SyntaxType::LambdaExp,
          AcceptableSyntax{SyntaxType::LambdaArgList}, expAcceptable,
      };
    }
    {
      syntax_['['] = Syntax{
          HierarchicalType::Parent, SyntaxType::LambdaArgList,
          AcceptableSyntax{SyntaxType::Variable},
          AcceptableSyntax{SyntaxType::LambdaArgListCont,
                           SyntaxType::LambdaArgListEnd},
      };
    }
    {
      syntax_['.'] = Syntax{
          HierarchicalType::Parent, SyntaxType::LambdaArgListCont,
          AcceptableSyntax{SyntaxType::Variable},
          AcceptableSyntax{SyntaxType::LambdaArgListCont,
                           SyntaxType::LambdaArgListEnd},
      };
    }
    {
      syntax_[']'] = Syntax{
          HierarchicalType::Leaf, SyntaxType::LambdaArgListEnd, nullAcceptable,
          nullAcceptable,
      };
    }
    {
      syntax_['+'] = Syntax{
          HierarchicalType::Parent, SyntaxType::OpExp, expAcceptable,
          expAcceptable,
      };
    }
    {
      syntax_['-'] = Syntax{
          HierarchicalType::Parent, SyntaxType::OpExp, expAcceptable,
          expAcceptable,
      };
    }
    {
      syntax_['='] = Syntax{
          HierarchicalType::Parent, SyntaxType::AssignExp,
          AcceptableSyntax{SyntaxType::Variable}, expAcceptable,
      };
    }
    {
      syntax_['<'] = Syntax{
          HierarchicalType::Parent, SyntaxType::OpExp, expAcceptable,
          expAcceptable,
      };
    }
    {
      syntax_['>'] = Syntax{
          HierarchicalType::Parent, SyntaxType::OpExp, expAcceptable,
          expAcceptable,
      };
    }
    {
      syntax_['?'] = Syntax{
          HierarchicalType::Parent, SyntaxType::IfExp, expAcceptable,
          AcceptableSyntax{SyntaxType::IfBranch},
      };
    }
    {
      syntax_['|'] = Syntax{
          HierarchicalType::Parent, SyntaxType::IfBranch, expAcceptable,
          expAcceptable,
      };
    }
    {
      syntax_[':'] = Syntax{
          HierarchicalType::Parent, SyntaxType::OpExp, expAcceptable,
          expAcceptable,
      };
    }
    {
      syntax_[EOF] = Syntax{
          HierarchicalType::Leaf, SyntaxType::None, nullAcceptable,
          nullAcceptable,
      };
    }
  }

  Node::Ref Parse(Input &input) {
    auto n = ParseNode(input);
    auto e = ParseNode(input);
    assert(e == nullptr);
    return n;
  }
  Node::Ref ParseNode(Input &input, int level = 0) {
    char c;
    for (;;) {
      c = input.Get();
      // skip space
      if (c == ' ' || c == '\t' || c == '\n') {
        continue;
      } else if (c == EOF) {
        return nullptr;
      }
      break;
    }

    assert(syntax_.find(c) != syntax_.end());

    auto syntax = syntax_[c];
    auto n = std::make_shared<Node>(Node{c});
    n->pos = input.pos();
    if (syntax.ht == HierarchicalType::Parent) {
      n->a = this->ParseNode(input, level + 1);
      n->d = this->ParseNode(input, level + 1);
    }
    return n;
  }
  bool Validate(Node::Ref n) {
    return ValidateNode(n, AcceptableSyntax{SyntaxType::Program});
  }
  bool ValidateNode(Node::Ref n, AcceptableSyntax as) {
    // validate current node
    bool valid = false;
    auto syntax = syntax_[n->c];
    for (auto asi : as) {
      if (asi == syntax.st) {
        valid = true;
      }
    }
    if (!valid) {
      printf("failed to validate at %c (%d,%d)\n", n->c, n->pos.line,
             n->pos.col);
      printf("must be either ");
      for (auto asi : as) {
        printf("%d ", asi);
      }
      printf("\n");
      return false;
    }

    // validate child node
    if (n->a) {
      if (!ValidateNode(n->a, syntax.aa)) {
        return false;
      }
    }
    if (n->d) {
      if (!ValidateNode(n->d, syntax.ad)) {
        return false;
      }
    }
    return true;
  }

  void Print(Node::Ref n, int level = 0) {
    for (int i = 0; i < level; i++) {
      printf(" ");
    }
    auto syntax = syntax_[n->c];
    printf("%c %d (%d,%d)\n", n->c, syntax.st, n->pos.line, n->pos.col);
    if (n->a) {
      Print(n->a, level + 1);
    }
    if (n->d) {
      Print(n->d, level + 1);
    }
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./l <program>\n");
    return 1;
  }

  Input input(argv[1]);
  if (!input.ok()) {
    printf("failed to open file\n");
    return 2;
  }

  char c;
  int level = 0;
  Lang l;

  auto n = l.Parse(input);
  l.Print(n);
  if (!l.Validate(n)) {
    return 3;
  }

  return 0;
}

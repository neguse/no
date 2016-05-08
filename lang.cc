#include <cstdio>
#include <cassert>
#include <memory>
#include <map>
#include <vector>
#include <iostream>

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

struct Node {
  typedef std::shared_ptr<Node> Ref;
  char c;
  Ref a;
  Ref d;
  Position pos;
  SyntaxType st;
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
      syntax_['!'] = Syntax{
          HierarchicalType::Parent, SyntaxType::AssignExp,
          AcceptableSyntax{SyntaxType::Variable}, expAcceptable,
      };
    }
    {
      syntax_['='] = Syntax{
          HierarchicalType::Parent, SyntaxType::OpExp, expAcceptable,
          expAcceptable,
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
    n->st = syntax.st;
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

int hex2i(char c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  } else {
    return (c - 'a') + 0xa;
  }
}

class LangInterpreter {

public:
  enum class ValueKind {
    Integer,
    List,
    Closure,
  };

  struct Value;
  struct Frame {
    typedef std::shared_ptr<Frame> Ref;
    Ref parent;
    std::map<char, std::shared_ptr<Value>> m;

    void Extend(char c, std::shared_ptr<Value> v) { m[c] = v; }
    void Set(char c, std::shared_ptr<Value> v) {
      auto it = m.find(c);
      if (it != m.end()) {
        it->second = v;
      } else {
        if (parent == nullptr) {
          m[c] = v;
        } else {
          parent->Set(c, v);
        }
      }
    }
    std::shared_ptr<Value> Get(char c) const {
      auto it = m.find(c);
      if (it != m.end()) {
        return it->second;
      } else if (parent != nullptr) {
        return parent->Get(c);
      } else {
        return nullptr;
      }
    }
  };
  struct Env {
    Frame::Ref root;
  };
  struct Cons {
    typedef std::shared_ptr<Cons> Ref;
    std::shared_ptr<Value> car;
    std::shared_ptr<Value> cdr;
  };
  enum class ClosureType {
    User = 0,
    I,
    S,
    A,
    D,
  };
  struct Closure {
    typedef std::shared_ptr<Closure> Ref;
    Frame::Ref e;
    Node::Ref p;
    ClosureType t;
  };
  struct Value {
    typedef std::shared_ptr<Value> Ref;
    ValueKind kind;
    struct { // unionにするとコンパイルエラーになる…
      int i;
      Cons::Ref l;
      Closure::Ref c;
    } v;
    int GetInteger() const {
      assert(kind == ValueKind::Integer);
      return v.i;
    }
    void SetInteger(int i) {
      kind = ValueKind::Integer;
      v.i = i;
    }
    Cons::Ref GetList() const {
      assert(kind == ValueKind::List);
      return v.l;
    }
    void SetList(Cons::Ref l) {
      kind = ValueKind::List;
      v.l = l;
    }
    Closure::Ref GetClosure() {
      assert(kind == ValueKind::Closure);
      return v.c;
    }
    void SetClosure(Closure::Ref c) {
      kind = ValueKind::Closure;
      v.c = c;
    }
    bool True() {
      switch (kind) {
      case ValueKind::Integer:
        return v.i != 0;
      case ValueKind::List:
        return v.c != nullptr;
      case ValueKind::Closure:
        return v.l != nullptr;
      }
      assert(0);
      return false;
    }
    void Print(int level = 0) const {
      for (int i = 0; i < level; i++) {
        printf(" ");
      }
      switch (kind) {
      case ValueKind::Integer: {
        printf("<Integer>%d\n", GetInteger());
      } break;
      case ValueKind::List: {
        if (v.l == nullptr) {
          printf("<List>NULL");
        } else {
          printf("<List>\n");
          printf("car:\n");
          GetList()->car->Print(level + 1);
          printf("cdr:\n");
          GetList()->cdr->Print(level + 1);
        }
        printf("\n");
      } break;
      case ValueKind::Closure:
        printf("<Closure>\n");
        break;
      }
    }
  };
  LangInterpreter() {}
  Value::Ref StringToList(const char *s) {
    if (*s == '\0') {
      Value::Ref v = std::make_shared<Value>();
      v->SetList(nullptr);
      return v;
    } else {
      Value::Ref v = std::make_shared<Value>();
      Cons::Ref c = std::make_shared<Cons>();
      Value::Ref vi = std::make_shared<Value>();
      vi->SetInteger(*s);
      c->car = vi;
      c->cdr = StringToList(s + 1);
      v->SetList(c);
      return v;
    }
  }
  Value::Ref ReadI() {
    int i = 0;
    std::cin >> i;
    Value::Ref v = std::make_shared<Value>();
    v->SetInteger(i);
    return v;
  }
  Value::Ref ReadS() {
    std::string s;
    std::cin >> s;
    Value::Ref v = StringToList(s.c_str());
    return v;
  }
  void PrintAsString(Value::Ref v) {
    if (v->kind == ValueKind::Integer) {
      printf("%d\n", v->GetInteger());
    } else if (v->kind == ValueKind::List) {
      for (auto row = v; row->GetList() != nullptr; row = row->GetList()->cdr) {
        for (auto col = row->GetList()->car; col->GetList() != nullptr;
             col = col->GetList()->cdr) {
          printf("%c", col->GetList()->car->GetInteger());
        }
        printf("\n");
      }
    }
  }
  void Exec(Node::Ref n) {
    Frame::Ref e = std::make_shared<Frame>();
    {
      Value::Ref v = std::make_shared<Value>();
      v->SetClosure(
          std::make_shared<Closure>(Closure{nullptr, nullptr, ClosureType::I}));
      e->Set('I', v);
    }
    {
      Value::Ref v = std::make_shared<Value>();
      v->SetClosure(
          std::make_shared<Closure>(Closure{nullptr, nullptr, ClosureType::S}));
      e->Set('S', v);
    }
    {
      Value::Ref v = std::make_shared<Value>();
      v->SetClosure(
          std::make_shared<Closure>(Closure{nullptr, nullptr, ClosureType::A}));
      e->Set('A', v);
    }
    {
      Value::Ref v = std::make_shared<Value>();
      v->SetClosure(
          std::make_shared<Closure>(Closure{nullptr, nullptr, ClosureType::D}));
      e->Set('D', v);
    }
    {
      Value::Ref v = std::make_shared<Value>();
      v->SetList(nullptr);
      e->Set('N', v);
    }
    auto v = Eval(n, e);
    // v->Print();
    PrintAsString(v);
  }
  Value::Ref Eval(Node::Ref n, Frame::Ref e) {
    switch (n->st) {
    case SyntaxType::Program:
    case SyntaxType::ProgramCont: {
      auto v = Eval(n->a, e);
      if (n->d->st == SyntaxType::ProgramEnd) {
        return v;
      } else {
        return Eval(n->d, e);
      }
    } break;
    case SyntaxType::AssignExp: {
      auto k = n->a->c;
      auto v = Eval(n->d, e);
      e->Extend(k, v);
      return v;
    } break;
    case SyntaxType::CallExp: {
      auto cv = Eval(n->a, e);
      auto c = cv->GetClosure();
      Frame::Ref f = std::make_shared<Frame>();
      f->parent = c->e;

      if (c->t == ClosureType::User) {

        auto larg = c->p->a;
        auto carg = n->d;
        for (;;) {
          if (larg->st == SyntaxType::LambdaArgListEnd &&
              carg->st == SyntaxType::CallArgListEnd) {
            break;
          }
          assert(larg->st != SyntaxType::LambdaArgListEnd);
          assert(carg->st != SyntaxType::CallArgListEnd);
          auto arg = Eval(carg->a, e);
          f->Extend(larg->a->c, arg);
          larg = larg->d;
          carg = carg->d;
        }
        return Eval(c->p->d, f);
      } else {
        switch (c->t) {
        case ClosureType::I:
          return ReadI();
        case ClosureType::S:
          return ReadS();
        case ClosureType::A: {
          auto l = Eval(n->d->a, e);
          return l->GetList()->car;
        }
        case ClosureType::D: {
          auto l = Eval(n->d->a, e);
          return l->GetList()->cdr;
        }
        }
        assert(0);
        return nullptr;
      }
    } break;
    case SyntaxType::LambdaExp: {
      Value::Ref v = std::make_shared<Value>();
      v->SetClosure(std::make_shared<Closure>(Closure{e, n}));
      return v;
    } break;
    case SyntaxType::OpExp: {
      auto a1 = Eval(n->a, e);
      auto a2 = Eval(n->d, e);
      Value::Ref v = std::make_shared<Value>();
      switch (n->c) {
      case '+':
        v->SetInteger(a1->GetInteger() + a2->GetInteger());
        return v;
      case '-':
        v->SetInteger(a1->GetInteger() - a2->GetInteger());
        return v;
      case '=':
        if (a1->kind != a2->kind) {
          v->SetInteger(0);
        } else if (a1->kind == ValueKind::Integer) {
          v->SetInteger(a1->GetInteger() == a2->GetInteger() ? 1 : 0);
        } else if (a1->kind == ValueKind::List) {
          v->SetInteger(a1->GetList() == a2->GetList() ? 1 : 0);
        }
        return v;
      case '<':
        v->SetInteger(a1->GetInteger() < a2->GetInteger() ? 1 : 0);
        return v;
      case '>':
        v->SetInteger(a1->GetInteger() > a2->GetInteger() ? 1 : 0);
        return v;
      case ':': {
        auto c = std::make_shared<Cons>(Cons{a1, a2});
        v->SetList(c);
        return v;
      }
      }
      assert(0);
      return nullptr;
    } break;
    case SyntaxType::IfExp: {
      auto cond = Eval(n->a, e);
      if (cond->True()) {
        return Eval(n->d->a, e);
      } else {
        return Eval(n->d->d, e);
      }
    } break;
    case SyntaxType::Hex2: {
      Value::Ref v = std::make_shared<Value>();
      int i = (hex2i(n->a->c) << 4) | hex2i(n->d->c);
      v->SetInteger(i);
      return v;
    } break;
    case SyntaxType::Hex: {
      Value::Ref v = std::make_shared<Value>();
      v->SetInteger(hex2i(n->c));
      return v;
    } break;
    case SyntaxType::Variable: {
      return e->Get(n->c);
    } break;
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
  // l.Print(n);
  if (!l.Validate(n)) {
    return 3;
  }

  auto i = LangInterpreter();
  i.Exec(n);

  return 0;
}

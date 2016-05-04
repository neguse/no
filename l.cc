#include <cstdio>
#include <cassert>
#include <memory>
#include <map>

struct Node {
  typedef std::shared_ptr<Node> Ref;
  char c;
  Ref a;
  Ref d;
};

void Print(Node::Ref n, int level = 0) {
  for (int i = 0; i < level; i++) {
    printf(" ");
  }
  printf("%c\n", n->c);
  if (n->a) {
    Print(n->a, level + 1);
  }
  if (n->d) {
    Print(n->d, level + 1);
  }
}

struct Syntax {
  char c;
  bool isLeaf;
};

class Lang {
private:
  std::map<char, Syntax> syntax_;

public:
  Lang() {
    for (char c = 'a'; c <= 'z'; c++) {
      syntax_[c] = Syntax{c, true};
    }
    for (char c = 'A'; c <= 'Z'; c++) {
      syntax_[c] = Syntax{c, true};
    }
    for (char c = '0'; c <= '9'; c++) {
      syntax_[c] = Syntax{c, true};
    }

    {
      char c = '{';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = ';';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '}';
      syntax_[c] = Syntax{c, true};
    }
    {
      char c = '#';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '~';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '\\';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '_';
      syntax_[c] = Syntax{c, true};
    }
    {
      char c = '(';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = ',';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = ')';
      syntax_[c] = Syntax{c, true};
    }
    {
      char c = '+';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '-';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '=';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '<';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '>';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '?';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = ':';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '#';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = '.';
      syntax_[c] = Syntax{c, false};
    }
    {
      char c = EOF;
      syntax_[c] = Syntax{c, true};
    }
  }

  Node::Ref Parse(FILE *fp) {
    auto n = ParseNode(fp);
    auto e = ParseNode(fp);
    assert(e == nullptr);
    return n;
  }
  Node::Ref ParseNode(FILE *fp, int level = 0) {
    char c;
    for (;;) {
      c = fgetc(fp);
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
    if (!syntax.isLeaf) {
      n->a = this->ParseNode(fp, level + 1);
      n->d = this->ParseNode(fp, level + 1);
    }
    return n;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./l <program>\n");
    return 1;
  }

  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL) {
    printf("failed to open file %s\n", argv[1]);
    return 2;
  }

  char c;
  int level = 0;
  Lang l;
  auto n = l.Parse(fp);
  Print(n);

  return 0;
}

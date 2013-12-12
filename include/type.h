#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>

enum TypeKind {
  TypeKind_First,
  TypeKind_Bool,
  TypeKind_Char,
  TypeKind_Short,
  TypeKind_Int,
  TypeKind_LongLong,
  TypeKind_Float,
  TypeKind_Double,
  TypeKind_Class,
  TypeKind_Last
};

extern std::vector<struct Class *> types;

struct Class {
  struct Field {
    std::vector<int> array_dimensions;
    TypeKind type;
    int field_i;
    int class_i;
    int bitfield_width;
    int alignment;
    int type_class;
    bool is_anonymous;
    Field(TypeKind ty, int fi, int ci)
        : type(ty), field_i(fi), class_i(ci), bitfield_width(-1), alignment(-1),
          type_class(-1), is_anonymous(false) {}

    Field &add_array_dimension(int array_dimension) {
      array_dimensions.push_back(array_dimension);
      return *this;
    }

    Field &set_bitfield_width(int width) {
      bitfield_width = width;
      if (width == 0)
        is_anonymous = true;
      int max_width = -1;
      if (type == TypeKind_Bool)
        max_width = 1;
      else if (type == TypeKind_Char)
        max_width = 8;
      else if (type == TypeKind_Short)
        max_width = 16;
      else if (type == TypeKind_Int)
        max_width = 32;
      else if (type == TypeKind_LongLong)
        max_width = 64;
      if (bitfield_width > max_width)
        bitfield_width = max_width;
      return *this;
    }

    Field &set_alignment(int align) {
      alignment = align;
      return *this;
    }

    Field &set_type_class(int ci) {
      type_class = ci;
      return *this;
    }

    Field &set_anonymous() {
      is_anonymous = true;
      return *this;
    }

    std::string get_field_name() const;

    friend std::ostream &operator<<(std::ostream &stream, const Field &field);
  };

  std::unordered_set<int> indirect_vbases;
  std::unordered_set<int> indirect_nvbases;
  std::unordered_set<int> direct_vbases;
  std::unordered_set<int> direct_nvbases;
  std::vector<int> direct_bases;
  std::vector<Field *> fields;
  std::vector<std::string> methods;
  int class_i;
  int alignment;
  int packed;

  Class(int ci) : class_i(ci), alignment(-1), packed(-1) {}

  bool can_inherit(int pbase) const;

  void add_base(int base, bool is_virtual);

  bool has_base(int base, bool is_virtual) const;

  Field &add_field(TypeKind tk) {
    fields.push_back(new Field(tk, fields.size(), class_i));
    return *fields.back();
  }

  void set_packed(int pack) {
    packed = pack;
  }

  void set_alignment(int align) {
    alignment = align;
  }

  void add_method(std::string method_name) {
    methods.push_back(method_name);
  }

  std::string get_class_name() const;

  friend std::ostream &operator<<(std::ostream &stream, const Class &type);
};

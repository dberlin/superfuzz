#include "type.h"

std::vector<Class *> types;

std::string Class::Field::get_field_name() const {
  std::stringstream field_name_stream;
  Class *record = types[class_i];
  int num_fields = record->fields.size();
  field_name_stream << record->get_class_name() << "FieldName" << field_i;
  return field_name_stream.str();
}

std::string Class::get_class_name() const {
  std::stringstream class_name_stream;
  class_name_stream << "ClassName" << class_i;
  return class_name_stream.str();
}

void Class::add_base(int base, bool is_virtual) {
  Class *base_type = types[base];
  indirect_vbases.insert(base_type->indirect_vbases.begin(),
                         base_type->indirect_vbases.end());
  indirect_nvbases.insert(base_type->indirect_nvbases.begin(),
                          base_type->indirect_nvbases.end());
  indirect_vbases.insert(base_type->direct_vbases.begin(),
                         base_type->direct_vbases.end());
  indirect_nvbases.insert(base_type->direct_nvbases.begin(),
                          base_type->direct_nvbases.end());
  direct_bases.push_back(base);
  if (is_virtual) {
    direct_vbases.insert(base);
  } else {
    direct_nvbases.insert(base);
  }
}

bool Class::has_base(int base, bool is_virtual) const {
  if (is_virtual) {
    return direct_vbases.count(base) != 0 || indirect_vbases.count(base) != 0;
  } else {
    return direct_nvbases.count(base) != 0 || indirect_nvbases.count(base) != 0;
  }
}

bool Class::is_viable_base(int new_base) const {
  Class *new_base_class = types[new_base];
  for (int direct_base : direct_bases) {
    if (direct_vbases.count(direct_base) != 0) {
      if (new_base_class->has_base(direct_base, /*is_virtual=*/false)) {
        return false;
      }
    } else {
      if (new_base_class->has_base(direct_base, /*is_virtual=*/true)) {
        return false;
      }
    }

    if (indirect_nvbases.count(direct_base) != 0 &&
        new_base_class->has_base(direct_base, /*is_virtual=*/true)) {
      return false;
    }
    if (indirect_vbases.count(direct_base) != 0 &&
        new_base_class->has_base(direct_base, /*is_virtual=*/false)) {
      return false;
    }
  }
  return true;
}
std::ostream &operator<<(std::ostream &stream, const Class &type) {
  if (type.packed > -1) {
    stream << "#pragma pack(push, " << type.packed << ")\n";
  }

  stream << "struct ";
  if (type.alignment > -1) {
    if (type.gnu_alignment_spelling) {
      stream << " __attribute__ ((aligned (" << type.alignment << "))) ";
    } else {
      stream << " __declspec(align(" << type.alignment << ")) ";
    }
  }
  stream << type.get_class_name();
  std::string base_clause;
  base_clause.append(": ");
  for (int direct_base : type.direct_bases) {
    base_clause.append("public ");
    if (type.direct_vbases.count(direct_base) != 0) {
      base_clause.append("virtual ");
    }
    base_clause.append(types[direct_base]->get_class_name());
    base_clause.append(", ");
  }
  base_clause.resize(base_clause.size() - 2);

  stream << base_clause << " {\n";
  for (auto &field : type.fields) {
    stream << '\t' << *field << '\n';
  }

  for (auto &method_name : type.methods) {
    stream << "\tvirtual void " << method_name << "() {}\n";
  }

  stream << '\t' << type.get_class_name() << "() {\n";
  for (auto field : type.fields) {
    if (field->is_anonymous) {
      continue;
    }
    if (field->bitfield_width > -1) {
      continue;
    }
    stream << "\t\tprintf(\"" << field->get_field_name()
           << " : %llu\\n\", (unsigned long long)((size_t)&"
           << field->get_field_name() << " - (size_t)buffer));\n";
  }
  stream << "\t}\n";
  stream << "};\n";
  if (type.packed > -1) {
    stream << "#pragma pack(pop)\n";
  }

  return stream;
}

std::ostream &operator<<(std::ostream &stream, const Class::Field &field) {
  if (field.alignment > -1) {
    if (field.gnu_alignment_spelling) {
      stream << " __attribute__ ((aligned (" << field.alignment << "))) ";
    } else {
      stream << "__declspec(align(" << field.alignment << ")) ";
    }
  }

  switch (field.type) {
    case TypeKind_Bool:     stream << "bool";      break;
    case TypeKind_Char:     stream << "char";      break;
    case TypeKind_Short:    stream << "short";     break;
    case TypeKind_Int:      stream << "int";       break;
    case TypeKind_LongLong: stream << "long long"; break;
    case TypeKind_Float:    stream << "float";     break;
    case TypeKind_Double:   stream << "double";    break;
    case TypeKind_Class:
      stream << types[field.type_class]->get_class_name();
      break;
    case TypeKind_PDM:
      stream << "int " << types[field.type_class]->get_class_name() << "::*";
  }
  if (!field.is_anonymous) {
    stream << ' ' << field.get_field_name();
  }
  for (auto dim : field.array_dimensions) {
    stream << '[' << dim << ']';
  }
  if (field.bitfield_width > -1) {
    stream << " : " << field.bitfield_width;
  }
  stream << ';';
  return stream;
}

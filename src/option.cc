#include "option.h"

#include <cstdlib>
#include <iostream>

OptionBase::~OptionBase() {}

std::unordered_map<std::string, OptionBase *> &get_option_map() {
  static std::unordered_map<std::string, OptionBase *> options;
  return options;
}

void parse_options(int argc, const char *argv[]) {
  const char *program_name = argv[0];

  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if (arg.find("--") == 0) {
      size_t equal_pos = arg.find('=');
      std::string opt_name;
      bool option_argument_given = equal_pos != std::string::npos;
      std::string option_argument;
      if (option_argument_given) {
        opt_name = arg.substr(2, equal_pos - 2);
        option_argument = arg.substr(equal_pos + 1);
      } else {
        opt_name = arg.substr(2);
      }
      auto options = get_option_map();
      auto option_pair = options.find(opt_name);
      if (option_pair == options.end()) {
        std::cerr << program_name << ": option " << opt_name
                  << " not found.\n";
        std::exit(EXIT_FAILURE);
      }
      OptionBase *option = option_pair->second;
      if (option->requires_argument() && !option_argument_given) {
        std::cerr << program_name << ": option " << opt_name
                  << " requires an argument.\n";
        std::exit(EXIT_FAILURE);
      }
      try {
        option->set_value(option_argument.c_str());
      } catch (...) {
        std::cerr << program_name << ": option " << opt_name
                  << " given malformed argument '" << option_argument << "'\n";
        std::exit(EXIT_FAILURE);
      }
    } else {
      usage(program_name);
      std::exit(EXIT_FAILURE);
    }
  }
}

void usage(const char *program_name) {
  std::cout << "usage: " << program_name;
  for (auto &option_pair : get_option_map()) {
    std::cout << " [--" << option_pair.first;
    if (option_pair.second->requires_argument()) {
      std::cout << "=value";
    }
    std::cout << ']';
  }
  std::cout << '\n';
}

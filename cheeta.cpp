#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>
#include <cstdlib> // for exit()

using Value = std::variant<int, std::string>;
std::unordered_map<std::string, Value> vars;
std::unordered_map<std::string, std::vector<std::string>> user_functions;
std::unordered_map<std::string, std::vector<std::string>> function_params;

std::string extract(const std::string& token) {
    size_t start = token.find("<get;");
    size_t end = token.find('>', start);
    if (start == std::string::npos || end == std::string::npos || end <= start + 5) {
        std::cerr << "[ERROR] Invalid <get;...> syntax: " << token << std::endl;
        return "";
    }
    return token.substr(start + 5, end - (start + 5));
}

int get_int(const std::string& name) {
    if (name.empty()) {
        std::cerr << "[ERROR] Cannot fetch variable: empty name." << std::endl;
        return 0;
    }
    if (!vars.count(name)) {
        std::cerr << "[ERROR] Variable '" << name << "' not found." << std::endl;
        return 0;
    }
    return std::get<int>(vars.at(name));
}

bool eval_condition_loop(const std::string& cond) {
    std::istringstream ss(cond);
    std::string left, op, right;

    ss >> left >> op >> right;

    if (left.empty() || op.empty() || right.empty()) {
        std::cerr << "[ERROR] Failed to parse condition into 3 parts: '" << cond << "'\n";
        return false;
    }

    std::cout << "[DEBUG] left = '" << left << "', op = '" << op << "', right = '" << right << "'\n";

    auto extract = [](const std::string& token) -> std::string {
        size_t s = token.find("<get;");
        size_t e = token.find('>', s);
        if (s == std::string::npos || e == std::string::npos || e <= s + 5) {
            std::cerr << "[ERROR] Invalid <get;...> syntax: " << token << std::endl;
            return "";
        }
        return token.substr(s + 5, e - (s + 5));
    };

    int l = 0, r = 0;

    try {
        l = (left.find("<get;") == 0 && left.back() == '>') ? get_int(extract(left)) : std::stoi(left);
        r = (right.find("<get;") == 0 && right.back() == '>') ? get_int(extract(right)) : std::stoi(right);
    } catch (...) {
        std::cerr << "[ERROR] Failed to evaluate left or right value\n";
        return false;
    }

    std::cout << "[DEBUG] eval_condition_loop: (" << l << " " << op << " " << r << ") => ";

    bool result = false;
    if (op == "<") result = l < r;
    else if (op == "<=") result = l <= r;
    else if (op == ">") result = l > r;
    else if (op == ">=") result = l >= r;
    else if (op == "==") result = l == r;
    else if (op == "!=") result = l != r;
    else {
        std::cerr << "[ERROR] Unknown operator: " << op << std::endl;
        return false;
    }

    std::cout << result << std::endl;
    return result;
}


std::string get_str(const std::string& name) {
    if (!vars.count(name)) {
        std::cerr << "[ERROR] Variable '" << name << "' not found.\n";
        return "";
    }
    return std::get<std::string>(vars.at(name));
}

bool eval_condition(const std::string& left_tok, const std::string& raw_op, const std::string& right_tok) {
    std::string op = raw_op;
    op.erase(0, op.find_first_not_of(" \t"));
    op.erase(op.find_last_not_of(" \t") + 1);

    auto extract = [](std::string token) -> std::string {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
    
        size_t start = token.find("<get;");
        size_t end = token.find('>', start);
        if (start == std::string::npos || end == std::string::npos || end <= start + 5) {
            std::cerr << "[ERROR] Invalid <get;...> syntax: " << token << std::endl;
            return "";
        }
        return token.substr(start + 5, end - (start + 5));
    };

    int left = get_int(extract(left_tok));
    int right = get_int(extract(right_tok));

    if (op == "==") return left == right;
    if (op == "!=") return left != right;
    if (op == "<")  return left < right;
    if (op == "<=") return left <= right;
    if (op == ">")  return left > right;
    if (op == ">=") return left >= right;

    std::cerr << "[ERROR] Unsupported operator: '" << op << "'\n";
    return false; // ✅ MUST include this
}


int eval_expr(const std::string& expr) {
    char op = 0;
    int left = 0, right = 0;

    size_t op_pos = expr.find_first_of("+-*/");
    if (op_pos == std::string::npos) return 0;

    op = expr[op_pos];
    std::string left_tok = expr.substr(0, op_pos);
    std::string right_tok = expr.substr(op_pos + 1);

    // Trim
    left_tok.erase(0, left_tok.find_first_not_of(" \t"));
    left_tok.erase(left_tok.find_last_not_of(" \t") + 1);
    right_tok.erase(0, right_tok.find_first_not_of(" \t"));
    right_tok.erase(right_tok.find_last_not_of(" \t") + 1);

    auto extract = [](const std::string& token) -> std::string {
        size_t s = token.find("<get;");
        size_t e = token.find('>', s);
        if (s == std::string::npos || e == std::string::npos || e <= s + 5) {
            std::cerr << "[ERROR] Invalid <get;...> syntax: " << token << std::endl;
            return "";
        }
        return token.substr(s + 5, e - (s + 5));
    };

    try {
        left = (left_tok.find("<get;") == 0 && left_tok.back() == '>') ?
               get_int(extract(left_tok)) : std::stoi(left_tok);
        right = (right_tok.find("<get;") == 0 && right_tok.back() == '>') ?
                get_int(extract(right_tok)) : std::stoi(right_tok);
    } catch (...) {
        std::cerr << "[ERROR] eval_expr failed: '" << expr << "'\n";
        return 0;
    }

    switch (op) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': return right != 0 ? left / right : 0;
    }
    return 0;
}


void fetch_std() {
    // No-op for now
}

void set_var(const std::string& name, int val) {
    vars[name] = val;
}

void call_print(int val) {
    std::cout << val << std::endl;
}

void call_exit(int code) {
    exit(code);
}

void interpret_line(const std::string& raw_line) {
    std::string clean = raw_line;
    clean.erase(0, clean.find_first_not_of(" \t"));

    if (clean.empty()) return;

    if (clean.find("fetch ") == 0) {
        std::string module = clean.substr(6);
        std::cout << "[DEBUG] Fetching module: " << module << std::endl;

        std::ifstream modfile("modules/" + module + ".cts");
        if (!modfile) {
            std::cerr << "[ERROR] Module not found: " << module << std::endl;
            return;
        }

        std::vector<std::string> modlines;
        std::string modline;
        while (std::getline(modfile, modline)) {
            modlines.push_back(modline);
        }

        for (size_t i = 0; i < modlines.size(); ++i) {
            std::string& line = modlines[i];

            if (line.find("define <") == 0) {
                size_t param_start = line.find('(');
                size_t param_end = line.find(')', param_start);

                auto start = line.find('<') + 1;
                auto end = line.find('>', start);
                std::string name = line.substr(start, end - start);

                std::vector<std::string> param_names;
                if (param_start != std::string::npos && param_end != std::string::npos) {
                    std::string param_str = line.substr(param_start + 1, param_end - param_start - 1);
                    std::istringstream param_stream(param_str);
                    std::string param;
                    while (std::getline(param_stream, param, ',')) {
                        param.erase(0, param.find_first_not_of(" \t"));
                        param.erase(param.find_last_not_of(" \t") + 1);
                        size_t semi = param.find(';');
                        std::string pname = param.substr(0, semi);
                        param_names.push_back(pname);
                    }
                }

                std::vector<std::string> body;
                ++i;
                while (i < modlines.size() && modlines[i] != "}") {
                    body.push_back(modlines[i]);
                    ++i;
                }

                user_functions[name] = body;
                function_params[name] = param_names;
                std::cout << "[DEBUG] Registered function: " << name << " with " << param_names.size() << " parameter(s)" << std::endl;
            } else {
                interpret_line(line);
            }
        }

    } else if (clean.find("set <") == 0) {
        auto start = clean.find('<') + 1;
        auto colon = clean.find(':', start);
        std::string name = clean.substr(start, colon - start);
        auto eq = clean.find("to");
        std::string expr = clean.substr(eq + 3);
        int val = eval_expr(expr);

        vars[name] = val;

    } else if (clean.find("call <std::print:function>") == 0) {
        auto lp = clean.find('(');
        auto rp = clean.find(')');
        auto expr = clean.substr(lp + 1, rp - lp - 1);

        if (expr.front() == '"' && expr.back() == '"') {
            std::cout << expr.substr(1, expr.size() - 2) << std::endl;
        } else if (expr.find("<get;") == 0) {
            size_t s = expr.find("<get;") + 5;
            size_t e = expr.find('>', s);
            std::string vname = expr.substr(s, e - s);
            
            if (vars.count(vname) && vars[vname].index() == 1) // string
                std::cout << std::get<std::string>(vars[vname]) << std::endl;
            else
                std::cout << get_int(vname) << std::endl;
        } else {
            std::cout << eval_expr(expr) << std::endl;
        }
        
        // std::cout << result << std::endl;

    } else if (clean.find("call <std::exit:function>") == 0) {
        auto lp = clean.find('(');
        auto rp = clean.find(')');
        int code = std::stoi(clean.substr(lp + 1, rp - lp - 1));
        exit(code);

    } else if (clean.find("call <") == 0 && clean.find("with parms") != std::string::npos) {
        auto start = clean.find('<') + 1;
        auto end = clean.find('>', start);
        std::string name = clean.substr(start, end - start);

        std::cout << "[DEBUG] Attempting to call user function: " << name << std::endl;

        size_t lp = clean.find('(', end);
        size_t rp = clean.find(')', lp);
        std::string arg_str = clean.substr(lp + 1, rp - lp - 1);

        std::vector<std::string> args;
        std::istringstream arg_stream(arg_str);
        std::string arg;
        while (std::getline(arg_stream, arg, ',')) {
            arg.erase(0, arg.find_first_not_of(" \t"));
            arg.erase(arg.find_last_not_of(" \t") + 1);
            args.push_back(arg);
        }

        if (user_functions.count(name)) {
            std::vector<std::string> params = function_params[name];
            std::unordered_map<std::string, Value> backup;

            for (size_t i = 0; i < params.size(); ++i) {
                std::string val = args[i];
            
                if (val.front() == '"' && val.back() == '"') {
                    // String literal
                    vars[params[i]] = val.substr(1, val.length() - 2);
                } else if (val.find("<get;") == 0) {
                    size_t s = val.find("<get;") + 5;
                    size_t e = val.find('>', s);
                    std::string vname = val.substr(s, e - s);
                    vars[params[i]] = vars[vname]; // preserve type
                } else {
                    try {
                        vars[params[i]] = std::stoi(val); // number literal
                    } catch (...) {
                        std::cerr << "[ERROR] Invalid argument: '" << val << "'\n";
                        return;
                    }
                }
            }
            

            for (const auto& inner : user_functions[name]) {
                std::cout << "[DEBUG] Executing: " << inner << std::endl;
                interpret_line(inner);
            }

            for (const auto& p : params) {
                if (backup.count(p)) vars[p] = backup[p];
                else vars.erase(p);
            }
        } else if (clean.find("call <") == 0 && clean.find("with parms") != std::string::npos) {
            auto start = clean.find('<') + 1;
            auto end = clean.find('>', start);
            std::string name = clean.substr(start, end - start);
        
            std::cout << "[DEBUG] Attempting to call user function: " << name << std::endl;
        
            size_t lp = clean.find('(', end);
            size_t rp = clean.find(')', lp);
            std::string arg_str = clean.substr(lp + 1, rp - lp - 1);
        
            std::vector<std::string> args;
            std::istringstream arg_stream(arg_str);
            std::string arg;
            while (std::getline(arg_stream, arg, ',')) {
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                args.push_back(arg);
            }
        
            if (user_functions.count(name)) {
                std::vector<std::string> params = function_params[name];
                if (args.size() != params.size()) {
                    std::cerr << "[ERROR] Argument count mismatch in call to: " << name << std::endl;
                    return;
                }
        
                std::unordered_map<std::string, Value> backup;
        
                for (size_t i = 0; i < params.size(); ++i) {
                    std::string val = args[i];
                
                    if (val.front() == '"' && val.back() == '"') {
                        // String literal
                        vars[params[i]] = val.substr(1, val.size() - 2); // remove quotes
                    } else if (val.find("<get;") == 0) {
                        size_t s = val.find("<get;") + 5;
                        size_t e = val.find(">", s);
                        std::string vname = val.substr(s, e - s);
                        vars[params[i]] = vars[vname]; // allow both string/int
                    } else {
                        vars[params[i]] = std::stoi(val); // fallback: number literal
                    }
                }
                
        
                for (const auto& inner : user_functions[name]) {
                    std::cout << "[DEBUG] Executing: " << inner << std::endl;
                    interpret_line(inner);
                }
        
                for (const auto& p : params) {
                    if (backup.count(p)) vars[p] = backup[p];
                    else vars.erase(p);
                }
            } else {
                std::cerr << "[ERROR] Unknown function: " << name << std::endl;
            }
        } else {
            std::cerr << "[WARN] Unhandled line: " << clean << std::endl;
        }        
    }
}

int main(int argc, char* argv[]) {

    if (argc >= 2 && std::string(argv[1]) == "mod") {
        std::string subcmd = argc >= 3 ? argv[2] : "";
    
        if (subcmd == "fetch" && argc == 4) {
            std::string module = argv[3];
            std::string url = "https://raw.githubusercontent.com/zimoshi/cheeta/main/modules/" + module + ".cts";
            std::string out = "modules/" + module + ".cts";
    
            std::cout << "[NOTE] Fetching module '" << module << "'...\n";
            std::string cmd = "curl -s -L " + url + " -o " + out;
            int res = system(cmd.c_str());
            if (res == 0)
                std::cout << "[DONE] Installed to " << out << "\n";
            else
                std::cerr << "[ERROR] Failed to fetch module.\n";
            return 0;
        }
        else if (subcmd == "list") {
            std::cout << "[INFO] Listing modules in ./modules:\n";
            for (const auto& entry : std::filesystem::directory_iterator("modules")) {
                std::cout << " - " << entry.path().filename() << "\n";
            }
            return 0;
        }
        else if (subcmd == "install" && argc == 4) {
            std::string pkg = argv[3];
            std::string cmd = "./octocatty install " + pkg;
            std::cout << "[NOTE] Calling octocatty to install '" << pkg << "'...\n";
            int res = system(cmd.c_str());
            return res;
        }
    
        std::cerr << "[ERROR] Unknown mod command.\n";
        return 1;
    } else if (subcmd == "info" && argc == 4) {
        std::string modname = argv[3];
        std::string path = "modules/" + modname + ".cts";
        std::ifstream modfile(path);
        if (!modfile) {
            std::cerr << "[ERROR] Module '" << modname << "' not found.\n";
            return 1;
        }
    
        std::map<std::string, std::string> meta;
        std::string line;
        while (std::getline(modfile, line)) {
            if (!line.starts_with("#")) break;
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(1, colon - 1);
                std::string value = line.substr(colon + 1);
                meta[key] = value;
            }
        }
    
        std::cout << "[INFO] Module: " << (meta["module"]) << "\n";
        std::cout << "[INFO] Version: " << (meta["version"]) << "\n";
        std::cout << "[INFO] Description: " << (meta["desc"]) << "\n";
        return 0;
    }    
    
    if (argc < 2) {
        std::cerr << "Usage: cheeta <script.ct>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    for (size_t i = 0; i < lines.size(); ++i) {
        std::string& line = lines[i];

        if (line.find("define <") == 0) {
            size_t param_start = line.find('(');
            size_t param_end = line.find(')', param_start);

            auto start = line.find('<') + 1;
            auto end = line.find('>', start);
            std::string name = line.substr(start, end - start);

            std::vector<std::string> param_names;
            if (param_start != std::string::npos && param_end != std::string::npos) {
                std::string param_str = line.substr(param_start + 1, param_end - param_start - 1);
                std::istringstream param_stream(param_str);
                std::string param;
                while (std::getline(param_stream, param, ',')) {
                    param.erase(0, param.find_first_not_of(" \t"));
                    param.erase(param.find_last_not_of(" \t") + 1);
                    size_t semi = param.find(';');
                    std::string pname = param.substr(0, semi);
                    param_names.push_back(pname);
                }
            }

            std::vector<std::string> body;
            ++i;
            while (i < lines.size() && lines[i] != "}") {
                body.push_back(lines[i]);
                ++i;
            }

            user_functions[name] = body;
            function_params[name] = param_names;
            std::cout << "[DEBUG] Registered function: " << name << " with " << param_names.size() << " parameter(s)" << std::endl;
            std::cout << "[DEBUG] Registered function: " << name << std::endl;
        } else if (line.find("loop ") == 0 && line.find("{") != std::string::npos) {
            std::string condition;
            std::vector<std::string> loop_body;
        
            // Extract condition from full line
            size_t brace_pos = line.find('{');
            condition = line.substr(5, brace_pos - 5);
            condition.erase(0, condition.find_first_not_of(" \t"));
            condition.erase(condition.find_last_not_of(" \t") + 1);
        
            std::cout << "[DEBUG] parsed loop condition: '" << condition << "'\n";
        
            ++i;
            while (i < lines.size() && lines[i] != "}") {
                loop_body.push_back(lines[i]);
                ++i;
            }
        
            while (eval_condition_loop(condition)) {
                for (const auto& l : loop_body) {
                    interpret_line(l);
                }
            }                   
        } else {
            interpret_line(line);
        }
        std::map<std::string, std::string> mod_meta;

        while (std::getline(modfile, line)) {
            if (line.starts_with("#")) {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string key = line.substr(1, colon - 1);
                    std::string value = line.substr(colon + 1);
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    mod_meta[key] = value;
                }
            } else {
                modlines.push_back(line); // actual code
            }
        }
        
    }

    return 0;
}

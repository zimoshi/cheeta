#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;
const std::string registry_path = "registry.json";
const std::string modules_dir = "modules/";

json load_registry() {
    std::ifstream in(registry_path);
    if (!in) {
        std::cerr << "[octocatty] ❌ Cannot open registry.json\n";
        exit(1);
    }
    json j;
    in >> j;
    return j;
}

void install(const std::string& pkg) {
    json reg = load_registry();

    if (!reg.contains(pkg)) {
        std::cerr << "[octocatty] ❌ Package not found in registry: " << pkg << "\n";
        return;
    }

    std::string url = reg[pkg];
    std::string out = modules_dir + pkg + ".cts";

    std::cout << "[octocatty] 📦 Installing '" << pkg << "' from:\n  " << url << "\n";
    std::string cmd = "curl -s -L " + url + " -o " + out;
    int res = system(cmd.c_str());

    if (res == 0)
        std::cout << "[octocatty] ✅ Installed to " << out << "\n";
    else
        std::cerr << "[octocatty] ❌ Failed to download.\n";
}

void list() {
    json reg = load_registry();
    std::cout << "[octocatty] 📚 Available packages:\n";
    for (auto& [k, v] : reg.items()) {
        std::cout << " - " << k << "\n";
    }
}

void info(const std::string& name) {
    std::string path = modules_dir + name + ".cts";
    std::ifstream file(path);
    if (!file) {
        std::cerr << "[octocatty] ❌ Module not found: " << name << "\n";
        return;
    }

    std::string line;
    std::cout << "[octocatty] ℹ️  Module header info:\n";
    while (std::getline(file, line)) {
        if (line.empty() || line[0] != '#') break;

        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(1, colon - 1);
            std::string val = line.substr(colon + 1);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t") + 1);
            std::cout << " - " << key << ": " << val << "\n";
        }
    }
}

void publish(const std::string& file) {
    std::string repo_url = "https://github.com/zimoshi/cheeta-community.git";
    std::string clone_dir = "/tmp/cheeta-pub";

    std::string filename = std::filesystem::path(file).filename();
    std::string modname = filename.substr(0, filename.find(".cts"));

    std::cout << "[octocatty] 🚀 Publishing module: " << filename << "\n";

    std::string clone_cmd = "rm -rf " + clone_dir + " && git clone " + repo_url + " " + clone_dir;
    if (system(clone_cmd.c_str()) != 0) {
        std::cerr << "[octocatty] ❌ Failed to clone repository.\n";
        return;
    }

    std::string copy_cmd = "cp " + file + " " + clone_dir + "/modules/";
    system(copy_cmd.c_str());

    std::string registry_file = clone_dir + "/registry.json";
    std::ifstream in(registry_file);
    json reg;
    in >> reg;
    in.close();

    std::string url = "https://github.com/zimoshi/cheeta-community/raw/refs/heads/main/modules/" + filename;
    reg[modname] = url;

    std::ofstream out(registry_file);
    out << reg.dump(2) << std::endl;
    out.close();

    std::string commit = "cd " + clone_dir + " && git add . && git commit -m 'Publish " + modname + "' && git push";
    int result = system(commit.c_str());

    if (result == 0)
        std::cout << "[octocatty] ✅ Published and registered '" << modname << "'\n";
    else
        std::cerr << "[octocatty] ❌ Push failed.\n";

    // Copy back updated registry to local Cheeta environment
    std::string copy_back_cmd = "cp " + registry_file + " ./registry.json";
    system(copy_back_cmd.c_str());

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: octocatty <install|list|info|publish> [args...]\n";
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "install" && argc == 3)
        install(argv[2]);
    else if (cmd == "list")
        list();
    else if (cmd == "info" && argc == 3)
        info(argv[2]);
    else if (cmd == "publish" && argc == 3)
        publish(argv[2]);
    else {
        std::cerr << "[octocatty] ❓ Unknown or invalid command.\n";
        return 1;
    }

    return 0;
}

#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>
#include "Indexer.h"
#include "SearchEngine.h"
#include "Ranker.h"

namespace fs = std::filesystem;
//Сценарий получения данных из недоступных репозиториев
void runScenario3() {
    std::cout << "[SCENARIO 3] Starting Path Filtering & Edge Cases Test...\n";

    Indexer indexer;

    std::cout << "Target: Non-existent folder testing...\n";
    indexer.indexDirectory("invalid_folder_name_xyz_123", {});
    assert(indexer.getIndex().getDocumentCount() == 0 && "Should be empty for invalid paths");

    std::string baseDir = "scenario_3_data";
    fs::create_directories(baseDir + "/allowed");
    fs::create_directories(baseDir + "/secret_trash");

    std::ofstream(baseDir + "/allowed/public.txt") << "important open source information";
    std::ofstream(baseDir + "/secret_trash/private.txt") << "important confidential clear text passwords";
    std::ofstream(baseDir + "/allowed/ignored_file.log") << "important system log dumps";

    std::vector<std::string> exclusions = {"secret_trash", "ignored_file.log"};

    std::cout << "Target: Indexing with exclusions...\n";
    indexer.indexDirectory(baseDir, exclusions);

    const auto& finalIndex = indexer.getIndex();

    std::cout << "Total indexed documents: " << finalIndex.getDocumentCount() << "\n";

    assert(finalIndex.getDocumentCount() == 1 && "Scenario 3 Failed: Exclusion filters did not work!");
    assert(finalIndex.getPostings("passwords").empty() && "Confidential data leaked into index!");
    assert(!finalIndex.getPostings("source").empty() && "Public data was not indexed!");

    fs::remove_all(baseDir);
    std::cout << "[SCENARIO 3] PASSED SUCCESSFULLY!\n\n";
}

int main() {
    runScenario3();
    return 0;
}
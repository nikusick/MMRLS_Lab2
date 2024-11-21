#include <iostream>
#include <filesystem>
#include <map>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;

class DirectoryResult {
public:
    DirectoryResult() = default;
    ~DirectoryResult() = default;

    DirectoryResult(const std::string& dir_path) : dirPath(dir_path) {}

    void addPayer(const std::string& payerName, int pSum) {
        payer = payerName;
        sum = pSum;
    }

    void addDebtor(const std::string& debName, int dSum) {
        debtorMap.insert({debName, dSum});
        debtSum += dSum;
    }

    friend std::ostream& operator<<(std::ostream& stream, const DirectoryResult& dirResult) {
        stream << "Директория: " << dirResult.dirPath << std::endl;
        stream << "Оплатил: " << dirResult.payer << std::endl;
        stream << "Сумма: " << dirResult.sum << std::endl;
        stream << "Должны вернуть: " << dirResult.debtSum << std::endl;
        if (dirResult.debtSum < dirResult.sum)
            stream << "Не хватает " << dirResult.sum- dirResult.debtSum << std::endl;
        else if (dirResult.debtSum > dirResult.sum)
            stream << "Лишнего " << dirResult.debtSum - dirResult.sum << std::endl;
        else stream << "Сумма потраченного совпадает с суммой долга" << std::endl;
        stream << "Должники:" << std::endl;
        for (const auto& debtor : dirResult.debtorMap) {
            stream << " - " << debtor.first << " ( " << debtor.second << " )" << std::endl;
        }
        return stream;
    }
private:
    std::string dirPath;
    std::string payer;
    int sum;
    int debtSum = 0;
    std::map<std::string, int> debtorMap;
};

class Result {
public:
    Result() = default;
    ~Result() = default;

    DirectoryResult& getDir(const std::string& dirName) {
        if (results.find(dirName) == results.end())
            results[dirName] = DirectoryResult(dirName);
        return results[dirName];
    }

    void addPayer(const std::string& dirName, const std::string& payerName, int pSum) {
        getDir(dirName).addPayer(payerName, pSum);
        if (people.find(payerName) == people.end()) {
            people[payerName] = {0, 0};
        }
        people[payerName].first += pSum;
    }

    void addDebtor(const std::string& dirName, const std::string& debName, int dSum) {
        getDir(dirName).addDebtor(debName, dSum);
        if (people.find(debName) == people.end()) {
            people[debName] = {0, 0};
        }
        people[debName].second += dSum;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Result& result) {
        stream << "Статистика: " << std::endl;
        stream << "------------------------------------------------------" << std::endl;
        for (const auto& person: result.people) {
            stream << "Имя: " << person.first << std::endl;
            stream << "Заплатил: " << person.second.first << std::endl;
            stream << "Должен остальным: " << person.second.second << std::endl;
            stream << "------------------------------------------------------" << std::endl;
        }
        for (const auto &dir: result.results) {
            stream << dir.second << std::endl;
        }
        return stream;
    }
private:
    std::map<std::string, DirectoryResult> results;
    std::map<std::string, std::pair<int, int>> people; // Имя, сколько заплатил, сколько должен
};


int main() {
    fs::path dir_path = fs::current_path() / "питер";
    auto result = Result();
    for (auto&& p: fs::recursive_directory_iterator(dir_path)) {
        if (p.is_directory()) continue;
        auto curDirectoryStr = p.path().parent_path().string();
        auto curFilename = p.path().stem().string();
        std::ifstream file(p.path());
        int sum;
        file >> sum;
        if (curFilename.find("Счет_") == 0)
            result.addPayer(curDirectoryStr, curFilename.substr(9), sum);
        else
            result.addDebtor(curDirectoryStr,curFilename, sum);
    }
    std::ofstream resultFile("..\\Results.txt");
    resultFile << result;
    return 0;
}

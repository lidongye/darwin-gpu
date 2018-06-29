#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "fasta.h"

static std::vector<std::string> SplitFields(std::string descrip_line) {
    std::vector<std::string> fields;
    std::string cur_str = "";
    for (int i = 1; i < descrip_line.length(); i++) {
        if (!isalpha(descrip_line[i]) && !isdigit(descrip_line[i]) && (descrip_line[i] != '_')) {
            fields.push_back(cur_str);
            cur_str = "";
        } else {
            cur_str.push_back(descrip_line[i]);
        }
    }
    fields.push_back(cur_str);

    return fields;
}

void ParseFastaFile(std::string filename,
        std::vector<std::vector<std::string> >& descrips,
        std::vector<std::string>& seqs,
        std::vector<long long int>& lengths,
        std::vector<long long int>& fileposs) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: Could not open FASTA file " << filename << "." << std::endl;
        exit(1);
    }

    int start_size = lengths.size();

    // First go through to get sequence descriptions, lengths, and file positions
    std::string line;
    long long int cur_length = 0;
    bool first_seq = true;
    long long int last_length;

    std::string curr_seq = "";

    while (getline(file, line)) {
        if (line.length() == 0) {
            continue;
        }

        if (line[0] == '>') {
            std::vector<std::string> fields = SplitFields(line);
            descrips.push_back(fields);
            fileposs.push_back(file.tellg());

            if (first_seq == false) {
                lengths.push_back(cur_length);
                cur_length = 0;
                seqs.push_back(curr_seq);
                curr_seq = "";
            }

            first_seq = false;
            last_length = SEQLINE_WRAP_LEN;
        } else {
            // Check file starts with description line
            if (first_seq == true) {
                std::cerr << "Error in file " << filename << ": File begins with non-description line!" << std::endl;
                return;
            }

            // Check sequence lines are wrapped to MAX_SEQLINE_LEN characters
            if ((line.length() > SEQLINE_WRAP_LEN) || (line.length() < SEQLINE_WRAP_LEN && last_length != SEQLINE_WRAP_LEN)) {
                std::cerr << "Error in file " << filename << ": FASTA sequence lines need to be wrapped to " << SEQLINE_WRAP_LEN << " characters!" << std::endl;
                std::cerr << "Keep in mind that sacCer3.fa has a wraplength of 50, and generate.sh generates fasta files with wraplength of 70" << std::endl;
                return;
            }

            curr_seq += line;
            cur_length += line.length();
            last_length = line.length();
        }
    }
    seqs.push_back(curr_seq);
    lengths.push_back(cur_length);

    file.close();
}


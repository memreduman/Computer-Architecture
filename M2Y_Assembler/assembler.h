#ifndef assembler_h
#define assembler_h
#include <string>
#pragma warning(disable : 4996) //disabled strcpy error for next line

struct instructions {
    char name[25];
    char type[5];
    uint8_t opcode;
    int16_t funct;
};

extern uint32_t inst_count;

extern instructions inst[];

struct label {
    char name[100];
    uint32_t address; //32-bit adress is defined.
};

extern uint32_t labelindex;
extern label label_list[100];

extern uint32_t datasection[100];
extern uint32_t datasection_index;

extern uint32_t instruction_memory_address[500];
extern uint32_t instr_index;

extern bool comments_removed_flag;
extern bool data_section_extracted_flag;
extern bool labes_extracted_flag;

void do_assembly(const std::string& inputFilename, const std::string& outputFilename);

void make_lower_case(const std::string& inputFilename, const std::string& outputFilename);

void removeSpacesFromFile(const std::string& inputFilename, const std::string& outputFilename);

void removeCommentsFromFile(const std::string& inputFilename, const std::string& outputFilename);

void findExactWord(const std::string& line, const std::string& search_word);

void find_directives_data(const std::string& inputFilename, const std::string& outputFilename);

void find_labels_text(const std::string& inputFilename, const std::string& outputFilename);

void insert_labels_addresses(const std::string& inputFilename, const std::string& outputFilename);

void assemble_line(const std::string& inputFilename, const std::string& outputFilename);



#endif
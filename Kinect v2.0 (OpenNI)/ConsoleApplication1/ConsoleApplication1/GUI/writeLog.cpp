#include "log.h"
 
using namespace std;

void write_text_to_log_file( const std::string &text )
{
    ofstream log_file(
        "log.txt", std::ios_base::out | std::ios_base::app );
    log_file << text << std::endl;
}

void flush_log(){
	ofstream log_file(
        "log.txt", std::ios_base::out | std::ios_base::app );
		log_file.flush();
		log_file.clear();
	
}
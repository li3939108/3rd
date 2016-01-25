#include "parser_helper.h"
#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

enum gate_type{
    INV = 323,
    NAND2,
    NAND3,
    NAND4,
    NAND5,
    NAND6,
    NAND7,
    NAND8,
    NAND9,
    NOR2,
    NOR3,
    NOR4,
    NOR5,
    NOR6,
    NOR7,
    NOR8,
    NOR9,
    XOR2,
    OA12,
    OA22,
    AO12,
    AO22,
    FFMS,
    AND2,
    AND3,
    OR2,
    OR3,
} ;

enum net_type {
	PI = 111 ,
	PO ,
	WIRE,
};

class Gate {
	string id ;
	enum gate_type type ;
	vector<class Gate > fanouts ;
	vector<class Gate > fanins ;
	vector<class Net > fanout_nets ;
	vector<class Net > fanin_nets ;
	Gate(string id, enum gate_type type);
};
class Net {
	string id ;
	enum net_type type ;
	string driver ;
	vector<string > load ;
public:
	Net(string id, enum net_type type);
	void print();
};

Gate::Gate(string id, enum gate_type type){
	this->id = string(id);
	this->type = type ;
}
Net::Net(string id, enum net_type type){
	this->id = string(id) ;
	this->type = type ;
}
void Net::print(){
	cout << "net: " << this->id << endl;
	cout << "type: " << (int)(this->type)  << endl;
}
int main(int argc, char **argv){
	vector<class Gate> gates ;
	vector<class Net> nets ;

	string net (128, 0);

	if(argc < 2){
		cerr << "Usage: ./3rd filename" << endl;
		return EXIT_FAILURE;
	}

	string verilog_filename(argv[1]);
	VerilogParser verilog_parser(verilog_filename);

	string moduleName ;
	bool valid = verilog_parser.read_module (moduleName) ;
	assert (valid) ;
  
	cout << "Module " << moduleName << endl << endl ;
	do {
		string primaryInput ;
		valid = verilog_parser.read_primary_input (primaryInput) ;

		if (valid){
			cout << "Primary input: " << primaryInput << endl ;
			nets.push_back( Net(primaryInput, PI) ) ;
		}
	} while (valid) ;
	cout << endl ;

	do {
		string primaryOutput ;
		valid = verilog_parser.read_primary_output (primaryOutput) ;

		if (valid){
			cout << "Primary output: " << primaryOutput << endl ;
			nets.push_back( Net(primaryOutput, PO) ) ;
		}

	} while (valid) ;
	cout << endl ;

	do {
		string net ;
		valid = verilog_parser.read_wire (net) ;

		if (valid){
			cout << "Net: " << net << endl ;
			nets.push_back( Net(net, WIRE) ) ;
		}

	} while (valid) ;


	cout << endl ;
	cout << "Cell insts: " << std::endl ;
	
	do {
		string cellType, cellInst ;
		vector<std::pair<string, string> > pinNetPairs ;
		
		valid = verilog_parser.read_cell_inst (cellType, cellInst, pinNetPairs) ;

		if (valid) {
			cout << cellType << " " << cellInst << " " ;
			for (int i=0; i < pinNetPairs.size(); ++i) {
				cout << "(" << pinNetPairs[i].first << " " << pinNetPairs[i].second << ") " ;
			}

			cout << endl ;
		}
		
	} while (valid) ;
	for(auto it = nets.begin(); it != nets.end() ; it ++){
		it->print() ;
	}
}

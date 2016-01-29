#include "parser_helper.h"
#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <map>
#include <vector>

using namespace std;

enum gate_type{
	ILLEGAL_GATE_TYPE = 323,
	DFF,
    INV ,
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
	AND4,
	AND5,
	AND6,
	AND7,
    OR2,
    OR3,
	OR4,
	OR5,
	OR6,
} ;

enum gate_type str2type(string t ){
	char *tok ;
	string tokstr ;
	const char *ct = t.c_str() ;
	char ctcopy[128] ;
	strcpy(ctcopy, ct);
	tok = strtok(ctcopy, "_") ;
	assert(tok != NULL) ;
	tokstr = string(tok) ;
	cout << "token " << tokstr << endl ;
	if(tokstr.substr(0, 2).compare( "OR" ) == 0  ){
		return (enum gate_type)(OR2 + tok[2] - '2') ;
	}else if(tokstr.substr(0, 3).compare("NOR") == 0  ){
		return (enum gate_type)(NOR2 + tok[3] - '2') ;
	}else if(tokstr.substr(0, 3).compare( "AND" ) == 0  ){
		return (enum gate_type)(AND2 + tok[3]  - '2' ) ;
	}else if(tokstr.substr(0, 3).compare( "INV" ) == 0 ){
		return INV;
	}else if(tokstr.substr(0, 3).compare( "DFF" ) == 0 ) {
		return DFF ;
	}else if(tokstr.substr(0, 4).compare( "NAND" ) == 0){
		return (enum gate_type)(NAND2 + tok[4] - '2' );
	}
	return ILLEGAL_GATE_TYPE; 
}

enum net_type {
	PI = 111 ,
	PO ,
	WIRE,
};
enum fanout_in {
	ILLEGAL_FAN = 1,
	FANOUT,
	FANIN ,
};
class Net {
	enum net_type type ;
	string driver ;
	vector<string > load ;
public:
	string id ;
	Net(string id, enum net_type type);
	Net() {} 
	void print();
};


class Gate {
	enum gate_type type ;
	vector<class Gate > fanouts ;
	vector<class Gate > fanins ;
	vector<class Net > fanout_nets ;
	vector<class Net > fanin_nets ;
public:
	string id ;
	Gate() {} 
	Gate(string id, enum gate_type type);
	void add_nets(map<string, class Net> nets_hash, string id, enum fanout_in ) ;
	void print() ;
};
void Gate::print(){
	cout << "Gate: " <<  this->id << " | Type: "<< (int)(this->type) << endl ;
	cout << "\tfanin : " ;
	for(auto it = this->fanin_nets.begin() ; it != this->fanin_nets.end() ; it++){
		cout << it->id << ", " ;
	}
	cout << "\n\tfanout : " ;
	for(auto it = this->fanout_nets.begin() ; it != this->fanout_nets.end() ; it++){
		cout << it->id << ", " ;
	}
	cout << endl ;
}
void Gate::add_nets(map<string, class Net> nets_hash, string id, enum fanout_in foi ) {
	if(foi == FANIN){
		this->fanin_nets.push_back( nets_hash[ id ] ) ;
	}else if(foi == FANOUT){
		this->fanout_nets.push_back( nets_hash[ id ] ) ;
	}
}
Gate::Gate(string id, enum gate_type type){
	this->id = string(id);
	this->type = type ;
}

Net::Net(string id, enum net_type type){
	this->id = string(id) ;
	this->type = type ;
}
void Net::print(){
	cout << "net: " << this->id << " | type: " << (int)(this->type)  << endl;
}
int main(int argc, char **argv){
	vector<class Gate> gates ;
	vector<class Net> nets ;
	std::map<string, class Net> nets_hash ;
	std::map<string, class Gate> gates_hash ;

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
			Net n(primaryInput, PI);
			cout << "Primary input: " << primaryInput << endl ;
			nets.push_back( n ) ;
			nets_hash[primaryInput] = n ;
		}
	} while (valid) ;
	cout << endl ;

	do {
		string primaryOutput ;
		valid = verilog_parser.read_primary_output (primaryOutput) ;

		if (valid){
			Net n(primaryOutput, PO) ;
			cout << "Primary output: " << primaryOutput << endl ;
			nets.push_back( n ) ;
			nets_hash[primaryOutput] = n ;
		}

	} while (valid) ;
	cout << endl ;

	do {
		string net ;
		valid = verilog_parser.read_wire (net) ;

		if (valid){
			Net n(net, WIRE) ;
			cout << "Net: " << net << endl ;
			nets.push_back( n ) ;
			nets_hash [ net ] = n ;
		}

	} while (valid) ;

	cout << endl ;
	cout << "Cell insts: " << std::endl ;
	
	do {
		string cellType, cellInst ;
		vector<std::pair<string, string> > pinNetPairs ;
		
		valid = verilog_parser.read_cell_inst (cellType, cellInst, pinNetPairs) ;

		if (valid) {
			Gate g (cellInst, str2type(cellType) ) ;
			cout << cellType << " " << cellInst << " " ;
			for (int i=0; i < pinNetPairs.size(); ++i) {
				if(pinNetPairs[i].first.compare("ZN" ) == 0|| 
				pinNetPairs[i].first.compare( "Q" ) == 0 ) {
					g.add_nets(nets_hash, pinNetPairs[i].second, FANOUT) ;
				}else{
					g.add_nets(nets_hash, pinNetPairs[i].second, FANIN) ;
				}
				cout << "(" << pinNetPairs[i].first << " " << pinNetPairs[i].second << ") " ;
			}
			gates.push_back(g ) ;
			gates_hash[ cellInst ] = g ;

			cout << endl ;
		}
		
	} while (valid) ;
	for(auto it = nets.begin(); it != nets.end() ; it ++){
		it->print() ;
	}
	for(auto it = gates.begin(); it != gates.end() ; it ++){
		it->print() ;
	}
}

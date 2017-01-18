#include <iostream>
#include <string>
#include <fstream>

using namespace std;

void decompress(string filename) {
		
	ifstream file(filename, ios::binary | ios::in);

	if (file.is_open()) {
		
		char N, L, S;
		file.read(&N, 1);
		file.read(&L, 1);
		file.read(&S, 1);
		cerr << int(N) << " " << int(L) << " " << int(S) << endl;
		string window = "";

		
		while (!file.eof()) {
			char len;
			file.read(&len, 1);

			char code = ((unsigned char)len) >> 4;

			cerr << (int)code << endl;


			if (code == 0) {
				char strlen = (len & 0x0f);
				
				if (strlen == 0)
					break;

				cerr << "strlen " << (int)strlen << endl;

				char* c_s = new char[strlen];
				file.read(c_s, strlen);

				string s(c_s, strlen);

				//cerr << s << endl;

				window += s;


				delete c_s;
			}
			else {
				char rest;
				file.read(&rest, 1);

				unsigned short offset = (len & 0x0f);
				offset <<= 8;
				offset |= (unsigned char)rest;


				cerr << "offset " << offset << endl;

				for (int i = window.length() - offset, c = 0; c < code; c++, i++){
					window += window[i];

				}
				
				//char t[2]; // 9-14 bits
				//t[1] = (char)data.front().strlen_offset;
				//t[0] = (char)(data.front().strlen_offset >> 8);

			}

			//cerr << window << endl;
		}

		cout << window;

	}
	else
		cerr << "File does not exist" << endl;
}
	


int main(int argc, char** argv) {
	if (argc != 2){
		cerr << "Invalid argument" << endl;
		return 1;
	}
	string filename = argv[argc - 1];
	cerr << filename << endl;
	decompress(filename);
	
	return 0;
}

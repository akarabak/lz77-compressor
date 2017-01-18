#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cmath>


using namespace std;

struct triple {
	char len;
	short strlen_offset;
	string s;
};



void fill(unordered_map<string, int>& dictionary, string window, int window_position, int step, int strlen) {
	step--;
	int start = max(0, (int)window.length() - strlen - 1);
	for (int i = start; i < window.length(); i++) {

		string one_to_right = window.substr(i, window.length());
		cerr << "inserting r: " << one_to_right << " at: " << window_position + i << endl;
		dictionary[one_to_right] = window_position + i;
		for (int j = 1; j <= step; j++) {
			cerr << "inserting l: " << one_to_right.substr(0, one_to_right.length() - j) << endl;
			dictionary[one_to_right.substr(0, one_to_right.length() - j)] = window_position + i;
		}
	}

}

void erase(unordered_map<string, int>& dictionary, string window, int amount_to_remove, int strlen, int window_position) {
	for (int i = 0; i < amount_to_remove; i++) {
		string one_right = window.substr(i, window.length());
		if (dictionary[one_right] < window_position)
			dictionary.erase(one_right);

		for (int j = 1; j <= one_right.length(); j++) {
			if (dictionary[one_right.substr(0, j)] < window_position)
				dictionary.erase(one_right.substr(0, j));
		}

	}
}

void compress(string filename, char N, char L, char S){

	int W = pow(2, N);
	int len = pow(2, L);

	ifstream file(filename, ios::binary | ios::in);

	deque<triple> data;

	if (file.is_open()) {

		int F_size = pow(2, L) - 1;
		char* F_buff = new char[F_size];
		int W = pow(2, N);
		int processed = 0;

		string window = "";
		string lookahead = ""; 

		

		unordered_map<string, int> dictionary;
		int window_position = 0;


		while (!file.eof() || lookahead != "") {

			if (!file.eof()) {
				file.read(F_buff, F_size - lookahead.length());
				lookahead.append(F_buff, F_size - lookahead.length());
			}

			
			string best_match = "";
			int offset = 0;
			for (int i = 2; i <= lookahead.size(); i++) { //looking for match in window (using hash map)
				string s = lookahead.substr(0, i);
				if (dictionary.find(s) != dictionary.end() && window_position - dictionary[s] <= W) {
					best_match = s;
					offset = dictionary[s];
				}
				else {
					
					if (dictionary[best_match] + best_match.length() == window.length()) { //check lookahead for extra match
						int count = 0;
						
						while (i + count <= lookahead.length() && best_match + lookahead[count] == s) {
							best_match += lookahead[count];
							if (i + count < lookahead.length())
								s = best_match + lookahead[i + count];
							count++;
						}
					}
					break;
				}
					
			}
			
			if (best_match == "")
				best_match = lookahead[0];

			if (best_match.length() < lookahead.length())
				lookahead = lookahead.substr(best_match.length(), lookahead.length());
			else
				lookahead = "";

			string o_window = window;
			window += best_match;

			if (window.length() > W) { //clearing hash table of old values and setting window position
				window_position += window.length() - W;

				erase(dictionary, o_window, window.length() - W, len, window_position);
				
				window = window.substr(window.length() - W, W);
			}


			fill(dictionary, window, window_position, best_match.length(), len);



			if (lookahead == "")
				break;

			if (best_match.length() == 1) {
				if (data.size() != 0 && data.back().len == 0 && data.back().strlen_offset < (int)pow(2, S)) {
					data.back().s += best_match;
					data.back().strlen_offset++;
				}
				else {
					triple temp;
					temp.len = 0;
					temp.s = best_match;
					temp.strlen_offset = 1;
					data.push_back(temp);
				}
			}
			else {
				triple temp;
				temp.len = best_match.length();
				temp.strlen_offset = offset;
				data.push_back(temp);

			}
			
			
		}

		delete F_buff;

		file.close();
	}


	cout.write(&N, 1);
	cout.write(&L, 1);
	cout.write(&S, 1);

	while (data.size() > 0) {
		if (data.front().len == 0) {
			cout.write(&data.front().len, 1);
			char t = data.front().strlen_offset; //1 - 3 bits for S
			cout.write(&t, 1);

			cout.write(data.front().s.c_str(), data.front().strlen_offset);
		}
		else {
			
			cout.write(&data.front().len, 1);

			char t[2]; // 9-14 bits
			t[0] = (char) data.front().strlen_offset;
			t[1] = (char) (data.front().strlen_offset >> 8);

			cout.write(t, 2);
		}
		data.pop_front();
	}

}

void decompress() {
	char N, L, S;
	cin >> N >> L >> S;
	cout << int(N) << " " << int(L) << " " << int(S) << endl;

}


int main(int argc, char **argv) {
	for (int i = 0; i < argc; i++) {
		string n = argv[i];
		cerr << argv[i] << endl;
		if (!n.find("-N")) {
			cerr << "found" << endl;
		}
	}

	char N = 11, L = 4, S = 3;

	compress("test.txt", N, L, S);


	return 0;
}
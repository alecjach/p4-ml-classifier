#include <iostream>
#include <map>
#include <set>
#include <string>


using namespace std;


// EFFECTS: Return a set of unique whitespace delimited words
set<string> unique_words(const string &str) {
  istringstream source(str);
  set<string> words;
  string word;
  while (source >> word) {
    words.insert(word);
  }
  return words;
}

class Classifier {
private:
    int total_posts = 0;

    //maps each label to a map of 

    //vocab
    set<string> vocab;
    
}




    //read posts and train





    //compute score of a post given a label, test with a bunch of labels and pick the best





int main() {
  //check for correct num arguments
    



    //read posts from a file








}






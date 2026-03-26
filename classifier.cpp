#include <iostream>
#include <map>
#include <set>
#include <string>
#include "csvstream.hpp"
#include <cmath>

using namespace std;




class Classifier {
    private:
    int num_posts = 0;

    set<string> vocab;
    
    //map word to number posts
    map<string, int> word_to_num_posts;
    
    //map label to number posts
    map<string, int> label_to_num_posts;
    
    //maps each label to a map of 
    //map label to (map word to num posts with word)
    map<string, map<string, int>> label_to_word_to_num_posts;


    public:
   
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


    void train_and_print_data_train_only(const string &filename){
      csvstream fin(filename);
      map<string, string> row;
      

      // Read tag and content for each row
      while (fin >> row) {
        num_posts++;
        string tag = row["tag"];
        cout << "\tlabel = " << tag;
        string content = row["content"];
        cout << ", content = " << content << endl;
        set<string> bag = unique_words(content);
        
        vocab.insert(bag.begin(), bag.end());

        for (auto word : bag){
          word_to_num_posts[word] ++;
          label_to_word_to_num_posts[tag][word] ++;
        }

        label_to_num_posts[tag] ++;
      }
      cout << "trained on " << num_posts << " examples" << endl;
      cout << "vocabulary size = " << vocab.size() << endl;
    }

   void train_and_print_data(const string &filename){
      csvstream fin(filename);
      map<string, string> row;
      

      // Read tag and content for each row
      while (fin >> row) {
        num_posts++;
        string tag = row["tag"];
        //cout << "label = " << tag;
        string content = row["content"];
        //cout << ", content = " << content << endl;
        set<string> bag = unique_words(content);
        
        vocab.insert(bag.begin(), bag.end());

        for (auto word : bag){
          word_to_num_posts[word] ++;
          label_to_word_to_num_posts[tag][word] ++;
        }

        label_to_num_posts[tag] ++;
      }
      cout << "trained on " << num_posts << " examples" << endl;
      //cout << "vocabulary size = " << vocab.size() << endl;
    }
    
    
    void train_only(){
      cout << "classes:" << endl;


      cout << "classifier parameters:" << endl;
      for (auto pair1 : label_to_word_to_num_posts){
        string lab = pair1.first;;
        for (auto pair2 : pair1.second){
          string word = pair2.first;
          int num = pair2.second;
          double log_p;//if word not in any post,  lnP(w|C) = 1 / total num posts 
          // if (vocab.find(word) == vocab.end()){
          //   log_p = log(1 / num_posts);
          // }
          // //if word not in labeled post, lnP(w|C) = num posts with word / total num posts 
          // if (label_to_word_to_num_posts[lab].find(word) == label_to_word_to_num_posts[lab].end()) {
          //   log_p = log(word_to_num_posts[word] / num_posts);
          // }

          //lnP(w|C) = num posts with label with word / num posts with label
          log_p = log(label_to_word_to_num_posts[lab][word] / label_to_num_posts[lab]);
          cout << "\t" << lab << ":" << word << ", count = "<< num
                << ", log-likelihood = " << log_p;
          
        }
      }

    }




    //return log probability P(C|X)
    double const log_prob(const string &label, const set<string> &bag){

      //lnP(C) = num posts with label / total posts;
      double total_log_prob = log(label_to_num_posts[label] /  num_posts); //keep adding after this


      //for every word in the word bag of a post,
      for (auto word : bag) {
        //if word not in any post,  lnP(w|C) = 1 / total num posts 
        if (vocab.find(word) == vocab.end()){
          total_log_prob += log(1 / num_posts);
        }
        //if word not in labeled post, lnP(w|C) = num posts with word / total num posts 
        if (label_to_word_to_num_posts[label].find(word) == label_to_word_to_num_posts[label].end()) {
          total_log_prob += log(word_to_num_posts[word] / num_posts);
        }

         //lnP(w|C) = num posts with label with word / num posts with label
         total_log_prob += log(label_to_word_to_num_posts[label][word] / label_to_num_posts[label]);
      }
      return total_log_prob;
     
    }

    
    string find_best_label(const set<string> &bag){
      double best_log_prob = 0.0;
      string best_label;
      for (auto pair : label_to_num_posts){
        string lab = pair.first;
        double temp_log_prob = log_prob(lab, bag);
        if (temp_log_prob < best_log_prob) {
          best_log_prob = temp_log_prob;
          best_label = lab;
        }
      }
   
      return best_label;
    }



  };





int main(int argc, char* argv[]) {
  cout.precision(3);
  
  if (argc != 2 && argc !=3) {
    cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
  }

  bool train_only = (argc == 2);

  string train = argv[1];
  ifstream train_file(train);
  if(!train_file.is_open()) {
    cout << "Error opening file: " << train << endl;
  }
 
  if (argc == 3) {
    string test = argv[2];
    ifstream test_file(test);
    if(!test_file.is_open()) {
      cout << "Error opening file: " << test << endl;
    }
  }
  

  Classifier ML;

  if (train_only) {
    ML.train_and_print_data_train_only(argv[1]);
  }
  else {
    ML.train_and_print_data(argv[2]);
  }
 


  ML.train_and_print_data("train_small.csv");
  



  //check for correct num arguments
    



    //read posts from a file








}






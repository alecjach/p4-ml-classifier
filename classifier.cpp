#include <iostream>
#include <sstream>
#include <fstream>
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
      
      cout << "training data:" << endl;

      // Read tag and content for each row
      while (fin >> row) {
        num_posts++;
        string tag = row["tag"];
        cout << "  label = " << tag;
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
      cout << endl;
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
      cout << endl;
    }
    
    
    void train_only(){
      cout << "classes:" << endl;


      
      for (auto pair : label_to_num_posts){
        cout << "  " << pair.first << ", " << pair.second << " examples, ";
        cout << "log-prior = " << log(static_cast<double>(pair.second) /  num_posts);
        cout << endl;
      }

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
          //if word not in labeled post, 
          //lnP(w|C) = num posts with word / total num posts 
          // if (label_to_word_to_num_posts[lab].find(word) 
          //== label_to_word_to_num_posts[lab].end()) {
          //   log_p = log(word_to_num_posts[word] / num_posts);
          // }

          //lnP(w|C) = num posts with label with word / num posts with label
          log_p = log(static_cast<double>(label_to_word_to_num_posts[lab][word]) 
                                                  / label_to_num_posts[lab]);
          cout << "  " << lab << ":" << word << ", count = "<< num
                << ", log-likelihood = " << log_p << endl;
          
        }
        
      }
      cout << endl;

    }




    //return log probability P(C|X)
    double const log_prob(const string &label, const set<string> &bag){

      //lnP(C) = num posts with label / total posts;
      double total_log_prob = 
        log(static_cast<double>(label_to_num_posts[label]) /  num_posts); 


      //for every word in the word bag of a post,
      for (auto word : bag) {
        //if word not in any post,  lnP(w|C) = 1 / total num posts 
        if (vocab.find(word) == vocab.end()){
          total_log_prob += log(1.0 / num_posts);
        } else
        //if word not in labeled post, lnP(w|C) = num posts with word / total num posts 
        if (label_to_word_to_num_posts[label].find(word) 
              == label_to_word_to_num_posts[label].end()) {
          total_log_prob += 
              log(static_cast<double>(word_to_num_posts[word]) / num_posts);
        } else{

         //lnP(w|C) = num posts with label with word / num posts with label
         total_log_prob += 
            log(static_cast<double>(label_to_word_to_num_posts[label][word]) 
                / label_to_num_posts[label]);
      
        }
      }
      return total_log_prob;
     
    }


    void test_data(const string &filename){
      csvstream fin(filename);
      map<string, string> row;

      int num_tested = 0;
      int num_correct = 0;
      
      cout << "test data:" << endl;

      // Read tag and content for each row
      while (fin >> row) {
        num_tested++;
        string tag = row["tag"];
        
        string content = row["content"];
        
        set<string> bag = unique_words(content);
        
        pair<string,double> predicted = find_best_label(bag);
        

        if (predicted.first == tag) {
          num_correct++;
        }

        cout << "  correct = " << tag;
        cout << ", predicted = " << predicted.first;
        cout << ", log-probability score = " << predicted.second << endl;;

        cout << "  content = " << content << endl << endl;
      }
      
      cout << "performance: " << num_correct << " / " 
            << num_tested <<  " posts predicted correctly" << endl;

    }
    
    pair<string, double> find_best_label(const set<string> &bag){
      double best_log_prob = -1000000000;
      string best_label;
      for (auto pair : label_to_num_posts){
        string lab = pair.first;
        double temp_log_prob = log_prob(lab, bag);
        if (temp_log_prob > best_log_prob) {
          best_log_prob = temp_log_prob;
          best_label = lab;
        }
      }
      pair<string, double> r;
      r.first = best_label;
      r.second = best_log_prob;
      return r;
    }



  };





int main(int argc, char* argv[]) {
  cout.precision(3);
  
  if (argc != 2 && argc !=3) {
    cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
    return -1;
  }

  bool train_only = (argc == 2);

  string train = argv[1];
  ifstream train_file(train);
  if(!train_file.is_open()) {
    cout << "Error opening file: " << train << endl;
    return -1;
  }
 
  if (argc == 3) {
    string test = argv[2];
    ifstream test_file(test);
    if(!test_file.is_open()) {
      cout << "Error opening file: " << test << endl;
      return -1;
    }
  }
  

  Classifier ML;

  if (train_only) {
    ML.train_and_print_data_train_only(train);
    ML.train_only();
  } else {
    ML.train_and_print_data(train);
    ML.test_data(argv[2]);
  }
 
}






// Working Memory Toolkit (WMtk)
// Adaptive Working Memory Library for Robot Control Systems
// Copyright (C) 2005, Joshua L. Phillips & David C. Noelle
// Department of Electrical Engineering and Computer Science
// Vanderbilt University; Nashville, Tennessee, USA.
// Email:  WMtk-Devel@ccnl.vuse.vanderbilt.edu

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

/******************************************************************************
 * WorkingMemory.cpp
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * The WorkingMemory class contains most of the interfaces that the user will
 * need to make use of. This class essentially contains all of the working
 * memory traces currently being maintained and also specifies all of the
 * functions for accessing and updating the working memory store. A single
 * instantiation of this class will create all of the instantiations of the
 * other objects necessary for proper working memory function. Thus, the user
 * will only need to provide the reward, state, and chunk evaluation functions
 * mentioned above to this class and this class will create any other
 * components required to process chunk information. However, the user will
 * need to provide candidate chunks to the system and will use the Chunk class
 * to accomplish this task.
 *
 * Joshua Phillips - Vanderbilt University - 5/25/04 3:05PM
 * 	Created source file.
 *
 * Joshua Phillips - Vanderbilt University - 5/25/04 4:56PM
 * 	Implemented constructors, destructor, assignment operator, and get/set
 * 	functions.
 *
 * Joshua Phillips - Vanderbilt University - 6/07/04 4:26PM
 *	Finished first pass at tickEpisodeClock.
 *
 * Joshua Phillips - Vanderbilt University - 6/17/04 3:25PM
 *	Finished canonical ordering code. This is accomplished by always
 *	keeping the memory contents in order so as to simplify the sorting
 *	process. Each time a new chunk is added, it is added in order.
 *
 * Joshua Phillips - Vanderbilt University - 7/06/04 4:30PM
 *	Finished updating code to accomodate the chunk_delete, chunk memory
 *	management function.
 *
 * Joshua Phillips - Vanderbilt University - 7/16/04 2:34PM
 *	Added code for various approaches to encoding chunk information. This
 *	code is commented out for now, but will be left here until deemed
 *	absolutely unnecessary.
 *
 * Joshua Phillips - Vanderbilt University - Sun Mar 19 23:45:31 2006
 *      Rewrote tick episode clock to include softmax combination
 *      selection and simplify OR and conjunctive code issues.
 *
 *****************************************************************************/

#include <WorkingMemory.h>
#include <cmath>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <list>
#include <vector>
#include <uniform_random_number_generator.h>
#include <nnet_math.h>
#include <Chunk.h>
#include <StateFeatureVector.h>
#include <AggregateFeatureVector.h>
#include <ActorNetwork.h>
#include <CriticNetwork.h>
#include <FeatureVector.h>
#include <ChunkFeatureVector.h>
#include <uniform_random_number_generator.h>

//#define OR_ONLY
//#define USE_SOFTMAX
#define SM_CONSTANT 5.0;

#include <iostream>
//#define WMTK_DEBUG

// Addtional structure for analyzing the combinations
struct WMCombo {
  int* chunks;
  double value;
};

void WorkingMemory::init() {
  state_vector_size = 0;
  chunk_vector_size = 0;
  number_of_chunks = 0;
  number_of_active_chunks = 0;
  working_memory_store = NULL;
  chunk_features = NULL;
  state_data_structure = NULL;
  actor_network = NULL;
  critic_network = NULL;
  reward_function = NULL;
  translate_state = NULL;
  translate_chunk = NULL;
  delete_chunk = NULL;
  episode_time = 0;
  use_actor = false;
  the_or_code = NO_OR;
  exploration_percentage = MEMORY_EXPLORATION_PERCENTAGE;
  last_reward = 0.0;
  or_vector = NULL;
  state_features = NULL;
  aggregate_features = NULL;

  return;
}

void WorkingMemory::dispose() {
  int x;

  if (working_memory_store != NULL) {
    for (x = 0; x < number_of_chunks; x++)
      if (working_memory_store[x] != NULL) {
	delete_chunk(*working_memory_store[x]);
	delete working_memory_store[x];
      }
    delete [] working_memory_store;
  }

  if (chunk_features != NULL)
    delete [] chunk_features;

  if (actor_network != NULL)
    delete actor_network;

  if (critic_network != NULL)
    delete critic_network;

  if (or_vector != NULL)
    delete or_vector;

  if (state_features != NULL)
    delete state_features;

  if (aggregate_features != NULL)
    delete aggregate_features;

  init();
  return;
}

void WorkingMemory::copy(const WorkingMemory& src, WorkingMemory& dest) {
  int x;
  dest.dispose();
  dest.state_vector_size = src.state_vector_size;
  dest.chunk_vector_size = src.chunk_vector_size;
  dest.number_of_chunks = src.number_of_chunks;
  dest.number_of_active_chunks = src.number_of_active_chunks;
  dest.working_memory_store = new Chunk*[src.number_of_chunks];
  dest.chunk_features = new ChunkFeatureVector*[src.number_of_chunks];
  for (x = 0; x < number_of_chunks; x++) {
    if (src.working_memory_store[x] != NULL)
      dest.working_memory_store[x] = new Chunk(*src.working_memory_store[x]);
    else
      dest.working_memory_store[x] = NULL;
    dest.chunk_features[x] = NULL;
  }
  dest.state_data_structure = src.state_data_structure;
  dest.state_features = new StateFeatureVector(*(src.state_features));
  dest.aggregate_features =
    new AggregateFeatureVector(*(src.aggregate_features));
  dest.critic_network = new CriticNetwork(*src.critic_network);
  // Leaving this one for later
  dest.actor_network = src.actor_network;
  dest.reward_function = src.reward_function;
  dest.translate_state = src.translate_state;
  dest.translate_chunk = src.translate_chunk;
  dest.delete_chunk = src.delete_chunk;
  dest.episode_time = src.episode_time;
  dest.use_actor = src.use_actor;
  dest.the_or_code = src.the_or_code;
  dest.exploration_percentage = src.exploration_percentage;
  dest.last_reward = src.last_reward;
  dest.or_vector = new FeatureVector(*(src.or_vector));
  return;
}

WorkingMemory::WorkingMemory() {
  init();
  return;
}

WorkingMemory::WorkingMemory(int wm_size,
			     int state_feature_vector_size,
			     int chunk_feature_vector_size,
			     void* user_state_data_structure,
			     double (*user_reward_function)(WorkingMemory&),
			     void (*user_state_function)(FeatureVector&, WorkingMemory&),
			     void (*user_chunk_function)(FeatureVector&, Chunk&,
							 WorkingMemory&),
			     void (*user_delete_function)(Chunk&),
			     bool use_actor,
			     OR_CODE or_code,
			     double mean_initial_values) {

  int x;

  if ((wm_size < 1) ||
      (state_feature_vector_size < 0) ||
      (chunk_feature_vector_size < 0) ||
      (user_state_data_structure == NULL && state_feature_vector_size < 1) ||
      (state_feature_vector_size == 0 && chunk_feature_vector_size == 0) ||
      (user_reward_function == NULL) ||
      (user_state_function == NULL) ||
      (user_chunk_function == NULL) ||
      (user_delete_function == NULL)) {

    init();
  }

  state_vector_size = state_feature_vector_size;
  chunk_vector_size = chunk_feature_vector_size;
  number_of_chunks = wm_size;
  number_of_active_chunks = 0;
  working_memory_store = new Chunk*[wm_size];
  chunk_features = new ChunkFeatureVector*[wm_size];
  for (x = 0; x < wm_size; x++) {
    working_memory_store[x] = NULL;
    chunk_features[x] = NULL;
  }
  if (state_feature_vector_size > 0)
    state_data_structure = user_state_data_structure;
  else
    state_data_structure = NULL;

  or_vector = new FeatureVector(chunk_vector_size + 1);

  state_features = new StateFeatureVector(state_feature_vector_size,
					  user_state_function);

#ifdef OR_ONLY
  aggregate_features =
    new AggregateFeatureVector(state_feature_vector_size,
			       chunk_feature_vector_size + 1, 0); // OR only
#else
  aggregate_features =
    new AggregateFeatureVector(state_feature_vector_size,
			       chunk_feature_vector_size + 1,
			       number_of_chunks); // Normal
#endif

  critic_network = new CriticNetwork(aggregate_features->getSize());
  UniformRandomNumberGenerator rng(mean_initial_values - 0.001,
				   mean_initial_values + 0.001);
  critic_network->initializeWeights(rng);
  // Leaving this one out until later
  actor_network = NULL;
  reward_function = user_reward_function;
  translate_state = user_state_function;
  translate_chunk = user_chunk_function;
  delete_chunk = user_delete_function;
  episode_time = 0;
  // Leaving this until later as well
  use_actor = false;
  the_or_code = or_code;
  exploration_percentage = MEMORY_EXPLORATION_PERCENTAGE;
  last_reward = 0.0;

  return;
}

WorkingMemory::~WorkingMemory() {
  dispose();
  return;
}

WorkingMemory::WorkingMemory(const WorkingMemory& L) {
  copy(L,*this);
  return;
}

WorkingMemory& WorkingMemory::operator=(const WorkingMemory& Rhs) {
  if (this != &Rhs) {
    copy(Rhs,*this);
  }
  return *this;
}

int WorkingMemory::getWorkingMemorySize() const {
  return number_of_chunks;
}

int WorkingMemory::getNumberOfChunks() const {
  return number_of_active_chunks;
}

Chunk WorkingMemory::getChunk(int chunk_number) const {

  Chunk result;

  if (chunk_number < 0 || chunk_number >= number_of_active_chunks)
    return result;

  return *working_memory_store[chunk_number];
}

void* WorkingMemory::getStateDataStructure() const {
  return state_data_structure;
}

bool WorkingMemory::newEpisode(bool clear_memory) {
  if (critic_network == NULL)
    return false;

  #ifdef WMTK_DEBUG
  cout << endl << "***** NEW EPISODE *****" << endl << endl;
  #endif

  int x, y;
  ChunkFeatureVector cfvector(chunk_vector_size + 1, translate_chunk);
  ChunkFeatureVector small_vector(chunk_vector_size, translate_chunk);
  cfvector.clearVector();
  cfvector.setValue(cfvector.getSize() - 1, WMTK_FV_MAX);

  // Need to absorb the reward from the last time step.
  critic_network->processVector(*aggregate_features);
  critic_network->processFinalTimeStep(last_reward);
  episode_time = 0;
  critic_network->clearEligibilityTraces();

  // Set up initial state
  state_features->updateFeatures(*this);
  last_reward = reward_function(*this);

  // Update WM contents and feature vectors
  if (clear_memory)
    number_of_active_chunks = 0;

  for (x = 0; x < number_of_chunks; x++) {
    if (clear_memory && working_memory_store[x] != NULL) {
      delete_chunk(*working_memory_store[x]);
      delete working_memory_store[x];
      working_memory_store[x] = NULL;
    }
    if (working_memory_store[x] != NULL) {
      chunk_features[x] = new ChunkFeatureVector(chunk_vector_size + 1,
						 translate_chunk);
      small_vector.clearVector();
      small_vector.updateFeatures(*(working_memory_store[x]), *this);
      for (y = 0; y < chunk_vector_size; y++)
	chunk_features[x]->setValue(y, small_vector.getValue(y));
    }
    else {
      chunk_features[x] = &cfvector;
    }
  }

  // Create OR code
  or_vector->clearVector();
  for (x = 0; x < number_of_active_chunks; x++)
    or_vector->makeORCode(*(chunk_features[x]), the_or_code);

  // Setup aggregate vector for first time step
  aggregate_features->updateFeatures(*state_features, chunk_features,
				    *or_vector);


  // Clear chunk_features (allocated memory)
  for (x = 0; x < number_of_chunks; x++) {
    if (chunk_features[x] != &cfvector)
      delete chunk_features[x];
    chunk_features[x] = NULL;
  }

  return true;
}

int WorkingMemory::getEpisodeTime() const {
  return episode_time;
}


// BIG NOTE::
// 	This function is supposed to run with either the critic only
//      or using an actor. For right now it will ONLY use the critic
//      by itself. The actor network portion has yet to be fully
//      understood for this problem.
//
int WorkingMemory::tickEpisodeClock(list<Chunk>& candidate_chunks, bool learn) {
  if (critic_network == NULL)
    return episode_time;

  int x, y, z;	// Local counters

  // Store the last time step vector for later processing
  AggregateFeatureVector* old_vector =
    new AggregateFeatureVector(*aggregate_features);

  // Update the state vector
  state_features->updateFeatures(*this);

  #ifdef WMTK_DEBUG
  cout << "***** BEGIN *****" << endl;
  cout << "Old Aggregate Vector: " << *old_vector << endl;
  cout << "New State Vector: " << *state_features << endl;
  cout << "Number of Chunks in Memory: " << number_of_active_chunks << endl;
  cout << "Number of Chunks in Candidate List: " << candidate_chunks.size()
       << endl;
  cout << "Old Memory: ";
  for (x = 0; x < number_of_chunks; x++)
    if (working_memory_store[x] == NULL)
      cout << "EMPTY ";
    else
      cout << working_memory_store[x]->getType() << " ";
  cout << endl;
  #endif

  // Copy candidate chunks list into vector of new objects
  vector<Chunk*> all_chunks;
  for (list<Chunk>::iterator itr = candidate_chunks.begin();
       itr != candidate_chunks.end(); itr++)
    all_chunks.push_back(new Chunk(*itr));

  // Empty candidate chunks list
  while (!candidate_chunks.empty())
    candidate_chunks.pop_front();

  // Add chunks currently in working memory to all_chunks vector
  for (x = 0; x < number_of_active_chunks; x++)
    all_chunks.push_back(working_memory_store[x]);

  // Clear WM contents
  number_of_active_chunks = 0;
  for (x = 0; x < number_of_chunks; x++)
    working_memory_store[x] = NULL;

  // Translate all chunks in all_chunks into feature vectors
  vector<ChunkFeatureVector> chunk_translations;
  ChunkFeatureVector cfvector(chunk_vector_size + 1, translate_chunk);
  ChunkFeatureVector small_cfvector(chunk_vector_size, translate_chunk);
  for (x = 0; x < all_chunks.size(); x++) {
    small_cfvector.clearVector();
    small_cfvector.updateFeatures(*(all_chunks[x]), *this);
    cfvector.clearVector();
    for (y = 0; y < chunk_vector_size; y++)
      cfvector.setValue(y, small_cfvector.getValue(y));
    chunk_translations.push_back(cfvector);
  }
  cfvector.clearVector(); // cfvector is now a NULL vector ("EMPTY")
  cfvector.setValue(cfvector.getSize() - 1, WMTK_FV_MAX);

  #ifdef WMTK_DEBUG
  cout << "VECTORS:" << endl;
  cout << "-1: " << cfvector << endl;
  for (x = 0; x < chunk_translations.size(); x++)
    cout << x << ": " << chunk_translations[x] << endl;
  #endif

  // Keep values of all combinations in vector
  vector<WMCombo*> combinations;
  WMCombo* combination_ptr = NULL;

  // Go through all combinations and store the values of the combinations.
  int* pass_counter = new int[number_of_chunks];
  for (x = 0; x < number_of_chunks; x++) {
    pass_counter[x] = -1;
  }

  bool carry = false;
  while (!carry) {
    bool duplicate = false;
    for (y = 0; y < number_of_chunks; y++)
      for (z = 0; z < number_of_chunks; z++)
	if (pass_counter[y] == pass_counter[z] && pass_counter[y] !=
	    -1 && y != z)
	  duplicate = true;

    // Do test on combination - no duplications allowed
    if (!duplicate) {
      combination_ptr = new WMCombo;
      combination_ptr->chunks = new int[number_of_chunks];
      for (y = 0; y < number_of_chunks; y++)
	combination_ptr->chunks[y] = pass_counter[y];

      // Assign feature vectors
      number_of_active_chunks = 0;
      for (x = 0; x < number_of_chunks; x++)
	if (combination_ptr->chunks[x] >= 0) {
	  number_of_active_chunks++;
	  chunk_features[x] = &chunk_translations[combination_ptr->chunks[x]];
	}
	else {
	  chunk_features[x] = &cfvector;
	}

      // Create OR code
      or_vector->clearVector();
      for (x = 0; x < number_of_active_chunks; x++)
	or_vector->makeORCode(*(chunk_features[x]), the_or_code);

      // Fill out aggregate feature vector
      aggregate_features->updateFeatures(*state_features, chunk_features,
					 *or_vector);
      combination_ptr->value =
	critic_network->processVector(*aggregate_features);

      // Store this combination and its value
      combinations.push_back(combination_ptr);
      combination_ptr = NULL;
    }

    // Increment combination counters
    carry = true;
    for (y = 0; y < number_of_chunks && carry; y++) {
      pass_counter[y]++;
      if (pass_counter[y] < all_chunks.size())
	carry = false;
      else
	pass_counter[y] = 0;
    }
  }
  delete [] pass_counter; // Finished with pass_counter

#ifdef WMTK_DEBUG
  cout << "COMBINATIONS" << endl;
  for (x = 0; x < combinations.size(); x++) {
    cout << x << ": " << combinations[x]->value << " | ";
    for (y = 0; y < number_of_chunks; y++) {
      if (combinations[x]->chunks[y] == -1)
	cout << "EMPTY ";
      else
	cout << all_chunks[combinations[x]->chunks[y]]->getType() << " ";
    }
    cout << endl;
  }
#endif

  int selection = 0; // Index of final (selected) combination
  double* values = new double[combinations.size()]; // Array of all values
  for (x = 0; x < combinations.size(); x++)
    values[x] = combinations[x]->value;

  // Choose the combination to use using the chosen method
#ifdef USE_SOFTMAX
  // Gibbs Softmax
  double* percentages = new double[combinations.size()];
  applySoftmax(SM_CONSTANT, combinations.size(), values, percentages);

  double c_percent = 0.0;
  double select_percent = 1.0 * rand() / (RAND_MAX + 1.0);
  for (x = 0; x < combinations.size(); x++) {
    c_percent += percentages[x];
    if (select_percent < c_percent) {
      selection = x;
      break;
    }
  }
  delete [] percentages; // Finished with percentages
#else
  // Epsilon-Greedy
  if ((1.0 * rand() / (RAND_MAX + 1.0)) < exploration_percentage)
    selection = (int) (((double) combinations.size()) * random() /
		       (RAND_MAX + 1.0));
  else
    selection = getIndexOfMaximum(combinations.size(), values);
#endif
  delete [] values; // Finished with values

  // Apply selected combination to memory
  combination_ptr = combinations[selection];

  // Assign feature vectors
  number_of_active_chunks = 0;
  for (x = 0; x < number_of_chunks; x++)
    if (combination_ptr->chunks[x] >= 0) {
      number_of_active_chunks++;
      chunk_features[x] = &chunk_translations[combination_ptr->chunks[x]];
      working_memory_store[x] =
	all_chunks[combinations[selection]->chunks[x]];
      // Once a chunk is used, make the pointer NULL
      // All non-null pointers in the vector will be deleted later
      all_chunks[combinations[selection]->chunks[x]] = NULL;
    }
    else {
      working_memory_store[x] = NULL;
      chunk_features[x] = &cfvector;
    }

  // Create OR code
  or_vector->clearVector();
  for (x = 0; x < number_of_active_chunks; x++)
    or_vector->makeORCode(*(chunk_features[x]), the_or_code);

  // Fill out aggregate feature vector
  aggregate_features->updateFeatures(*state_features, chunk_features,
				     *or_vector);

#ifdef WMTK_DEBUG
  cout << "Selected Combination: " << selection << endl;
  cout << "OR vector: " << *or_vector << endl;
  cout << "New Aggregate Vector: " << *aggregate_features << endl;
  cout << "***** END *****" << endl << endl;
#endif

  // Clean combinations
  for (x = 0; x < combinations.size(); x++) {
    delete [] combinations[x]->chunks;
    delete combinations[x];
  };

  // Clean all unused chunks
  for (x = 0; x < all_chunks.size(); x++)
    if (all_chunks[x] != NULL) {
      delete_chunk(*(all_chunks[x]));
      delete all_chunks[x];
    }

  // Clear chunk_features (all point to vectors on the stack)
  for (x = 0; x < number_of_chunks; x++)
    chunk_features[x] = NULL;

  // Learning
  // Process the old vector
  critic_network->processVector(*old_vector);
  critic_network->processVectorAsNextTimeStep(*aggregate_features,
					      last_reward,
					      learn);
  // Get reward value needed for later.
  last_reward = reward_function(*this);
  delete old_vector;

  return ++episode_time;
}

bool WorkingMemory::isUsingActor() const {
  return use_actor;
}

OR_CODE WorkingMemory::getORCode() const {
  return the_or_code;
}

bool WorkingMemory::setORCode(OR_CODE or_code) {
  the_or_code = or_code;
  return true;
}

bool WorkingMemory::saveNetwork(string filename) {
  if (critic_network == NULL)
    return false;

  ofstream output;
  output.open(filename.c_str());

  if (output.bad())
    return false;

  return critic_network->writeWeights(output);
  //return critic_network->writeWeights(output) &&
  //		 actor_network->writeWeights(output);
}

bool WorkingMemory::loadNetwork(string filename) {
  if (critic_network == NULL)
    return false;

  ifstream input;
  input.open(filename.c_str());

  if (!input.is_open() || input.bad())
    return false;

  return critic_network->readWeights(input);
  //return critic_network->readWeights(input) &&
  //		 actor_network->readWeights(input);
}

bool WorkingMemory::setExplorationPercentage(double val) {
  if (val >= 0.0 && val <= 100.0) {
    exploration_percentage = val;
    return true;
  }

  return false;
}

double WorkingMemory::getExplorationPercentage() const {
  return exploration_percentage;
}

CriticNetwork* WorkingMemory::getCriticNetwork() const {
  return critic_network;
}

bool WorkingMemory::checkForTickCall(double tolerance) {
  if (tolerance < 0.0)
    tolerance = 0.1;

  StateFeatureVector new_vector(state_features->getSize(), translate_state);

  new_vector.updateFeatures(*this);

  double total = 0.0;
  for (int x = 0; x < new_vector.getSize(); x++) {
    double temp = (new_vector.getValue(x) - state_features->getValue(x)) *
      (new_vector.getValue(x) - state_features->getValue(x));
    total += temp;
  }
  total = sqrt(total);

  if (total <= tolerance)
    return false;

  return true;
}

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
 * WorkingMemory.h
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
 * JLP - 5/4/04 8:59PM
 * 		Created stub class for developing FeatureVector class
 *              and subclasses.
 *
 * JLP - 5/25/04 3:45PM
 * 		Finished complete header file as in specs.
 *
 * JLP - 6/07/04 1:37PM
 *		Added generateChunkFeatureVectors method.
 *
 * JLP - 7/06/04 4:25PM
 *		Finished adding chunk memory management function
 *              (chunk_delete).
 *
 * Joshua Phillips - Vanderbilt University - Sun Mar 19 23:55:21 2006
 *      Made minor changes to private data structures and public
 *      interface. These include making the WM store and CFV store
 *      dynamically allocated and including an optional parameter to
 *      the main constructor for optimistic initial weights (exploration
 *      encouragement).
 *
 *****************************************************************************/

#ifndef WMTK_WORKING_MEMORY_H
#define WMTK_WORKING_MEMORY_H

#include <list>

#include <Chunk.h>
#include <StateFeatureVector.h>
#include <AggregateFeatureVector.h>

class ActorNetwork;
class CriticNetwork;
class FeatureVector;
class ChunkFeatureVector;
class UniformRandomNumberGenerator;

#define WMTK_MAX_CHUNKS	200
#define MEMORY_EXPLORATION_PERCENTAGE 0.05

using namespace std;

class WorkingMemory {

 public:

  // Constructor
  WorkingMemory();

  // Constructor
  //
  // The wm_size is the maximum number of chunks that can be stored in
  // the working memory system.
  //
  // The state_feature_vector and chunk_feature_vector parameters are
  // the size of the FeatureVectors that are needed by the
  // user_state_function and user_chunk_function functions,
  // respectively.
  //
  // The using_actor flag tells the working memory system whether to use
  // an actor network for selecting chunks for storage.
  // The using_or_code flag tells the working memory system whether to
  // evaluate the “OR” of all chunks or just take them individually when
  // selecting them for storage.
  WorkingMemory(int wm_size,
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
		double mean_initial_values = 2.0);

  // Destructor
  ~WorkingMemory();

  // Copy-Constructor
  WorkingMemory(const WorkingMemory&);

  // Assignment Operator
  WorkingMemory& operator=(const WorkingMemory&);

  // Returns the total number of chunks that the working memory system
  // can hold at any one time.
  int getWorkingMemorySize() const;

  // Returns the number of chunks currently stored in the working memory
  // store.
  int getNumberOfChunks() const;

  // Retrieves the specified chunk from working memory.
  Chunk	getChunk(int chunk_number) const;

  // Retrieves the user-provided data structure that contains  all
  // relevant information about the current state.
  void*	getStateDataStructure() const;

  // Resets the working memory class's time-step clock to zero. It
  // should be used at the end of every training “episode”. (It cues the
  // working memory class that this is the final state of the episode.)
  // A provided flag tells whether to clear all of the chunks out of
  // working memory. This function returns true if the new episode could
  // be properly initialized, false otherwise. (Reset the
  // environment/state information for the next episode *BEFORE* calling
  // this function, as it intializes the WM system using the current
  // state configuration as a start-state.)
  bool newEpisode(bool clear_memory = true);

  // Returns the current time step as recorded by the working memory
  // clock. Clock time starts at zero.
  int getEpisodeTime() const;

  // This function makes a comparison between the current state
  // representation and the one for the last time step to see if any
  // significant changes have occurred. If there has been a
  // significant change then it returns true, false otherwise. The
  // comparison is made by calculating the Euclidean distance between
  // the two state vectors. The provided tolerance will be used to
  // gauge the difference being significant. This function can be used
  // to gauge when to call tickEpisodeClock (it might not need to be
  // called if this function returns false.)
  bool checkForTickCall(double tolerance);

  // This function performs several different critical functions. First,
  // it calls the user-provided state feature extraction function, then
  // it calls the user-provided chunk feature extraction function for
  // each chunk in the provided list, and, finally, the actor-critic
  // networks are consulted and relevant chunks are added to the working
  // memory store. This essentially is the function that performs all of
  // the learning and memory updating. It then increments the working
  // memory clock and returns the new clock value. The clock will not
  // increment if there was a problem while processing working memory.
  int tickEpisodeClock(list<Chunk>& candidate_chunks, bool learn = true);

  // Checks to see if the WorkingMemory class is using an actor network
  // to process chunks.
  bool isUsingActor() const;

  // Returns the OR_CODE value corresonding to the OR_CODE the
  // WorkingMemory class is using.
  OR_CODE getORCode() const;

  // Sets the OR_CODE value for the WorkingMemory class.
  bool setORCode(OR_CODE or_code);

  // Writes the connection weight values of the neural networks to the
  // file, filename.
  bool saveNetwork(string filename);

  // Reads the connection weight values for the neural networks from the
  // file, filename.
  bool loadNetwork(string filename);

  // Returns the current memory updating exploration percentage being
  // used by the system.
  double getExplorationPercentage() const;

  // Sets the current memory exploration percentage. Returns true on
  // success and false otherwise.
  bool setExplorationPercentage(double val);

  // Returns a pointer to the CriticNetwork object that the
  // WorkingMemory object has created for learning the value of its
  // working memory contents.
  CriticNetwork* getCriticNetwork() const;

  // Returns a pointer to the ActorNetwork object that the WorkingMemory
  // object has created for learning when to add a  chunk to the working
  // memory system.
  ActorNetwork*	getActorNetwork() const;

 private:

  // The size of the state vector.
  int state_vector_size;

  // The size of a chunk vector.
  int chunk_vector_size;

  // Array of chunks stored in working memory.
  Chunk** working_memory_store;

  // Total size of the working memory (maximum number of chunks that can
  // be stored in working_memory_store.)
  int number_of_chunks;

  // Number of chunks currently held in working memory.
  int number_of_active_chunks;

  // Pointer to the data structure containing the current state's
  // information.
  void* state_data_structure;

  // Neural network architecture  for the action selection network.
  ActorNetwork* actor_network;

  // Neural network architecture  for the evaluation of the current
  // situation.
  CriticNetwork* critic_network;

  // Vector object that contains the processed state information.
  StateFeatureVector* state_features;

  // Vector objects that contain the processed chunk information.
  ChunkFeatureVector** chunk_features;

  // Vector object to contain the OR_CODE vector.
  FeatureVector* or_vector;

  // Vector object that can be processed by the actor and critic
  // networks.
  AggregateFeatureVector* aggregate_features;

  // Function for getting instantaneous reward information from the
  // user.
  double (*reward_function)(WorkingMemory&);

  // Function for translating the current state information into feature
  // vector form.
  void (*translate_state)(FeatureVector&, WorkingMemory&);

  // Function for translating chunks into feature vector form.
  void (*translate_chunk)(FeatureVector&, Chunk&, WorkingMemory&);

  // Function for deleting arbitrary chunk information before Chunk
  // disposal.
  void (*delete_chunk)(Chunk&);

  // Time step for the current episode.
  int episode_time;

  // Flag specifying whether to use the actor network to choose actions.
  bool use_actor;

  // Value specifying whether to use OR coding of the chunk feature
  // vectors when processing states and chunks.
  OR_CODE the_or_code;

  // Value of reward on last time step.
  double last_reward;

  // Percentage of the time that the WorkingMemory system performs a
  // memory update or release that is contrary to the one advised by the
  // critic network. (5% by default.)
  double exploration_percentage;

  // Contructor and Destructor utility functions
  void init();
  void dispose();
  void copy(const WorkingMemory&, WorkingMemory&);

};

#endif

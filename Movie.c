/*****************************************************
 * @file   Movie.c                                   *
 * @author Nikoletta Arvaniti <csd4844@csd.uoc.gr    *
 *                                                   *
 * @brief Implementation for Movie.h 				 *
 * Project: Winter 2023						         *
 *****************************************************/
#include "Movie.h"

int movieOccurrences[6] = {0};	/*helping array to count how many movies we have in each category*/
new_movie_t *array; /*helping pointer array for the distribute function*/
int indices[6] = {0}; /*helping array for distribute*/
movie_t *categoryBsts[6]; /*helping array for distribute*/
movie_t *foundMovie = NULL; /*for the watch function i think?*/
int numMovies = 0; /*for the filter function*/
movie_t *scoreArray; /*for the filter function*/
int scoreSum = 0, counter = 0;


 int hashfunction(int userId){
    int i, a, result;

    i = userId % 170;
	a = primes_g[i];

    result = (a * userId) % hashtable_size;

	return result;
 }

 void InOrder(new_movie_t *p){
	if(p == NULL){
		return;
	}
	InOrder(p->lc);
	printf("<%d>, ", p->movieID);
	InOrder(p->rc);
 }

 int register_user(int userID){

	int hash_i;
	user_t *current = NULL;

	hash_i = hashfunction(userID);
	current = user_hashtable_p[hash_i];
   
	while(current != NULL){
		if(current->userID == userID){
			printf("User %d already exists.\n", userID);
			return 0;
		}
		current = current->next;
	}

	user_t *new = (user_t *)malloc(sizeof(user_t));
	if(new == NULL){
		return 0;
	}

	new->userID = userID;
	new->next = NULL;
	new->history = NULL;

	
	new->next = user_hashtable_p[hash_i];
	user_hashtable_p[hash_i] = new;

	printf("Chain <%d> of Users:\n", hash_i);
	current = user_hashtable_p[hash_i];
	while(current != NULL){
		printf("\t<%d>\n", current->userID);
		current = current->next;
	}

	 return 1;
 }
 


/**
 * @brief Deletes a user.
 * Deletes a user with userID from the system, along with users' history tree.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */

 int unregister_user(int userID){

	int hash_i, flag = 0;
	user_t *current, *prev;

	hash_i = hashfunction(userID);

	current = user_hashtable_p[hash_i];
	prev = NULL;

	while(current != NULL){
		if(current->userID == userID){
            flag = 1;
			if(prev == NULL){
				user_hashtable_p[hash_i] = current->next;
			}else{
				prev->next = current->next;
			}
			free(current);
		}
		prev = current;
		current = current->next;
	}

	if(flag == 1){
        current = user_hashtable_p[hash_i];
	    printf("Chain <%d> of Users:\n", hash_i);
        if(current == NULL){
            printf("\t<0>\n");
        }
	    while(current != NULL){
		    printf("\t<%d>\n", current->userID);
		    current = current->next;
	    }

	    return 1;
    }

    printf("User <%d> does not exist.\n", userID);
    return 0;

 }


 int add_new_movie(int movieID, int category, int year){

	/*Making a new tree node*/
	new_movie_t *newMovie = (new_movie_t *)malloc(sizeof(new_movie_t));
	if(newMovie == NULL){
		return 0;
	}

	newMovie->movieID = movieID;
	newMovie->category = category;
	newMovie->year = year;
	newMovie->sumScore = 0;
	newMovie->watchedCounter = 0;

	if(new_releases == NULL){
		new_releases = newMovie; /*the first element becomes the root of the bst*/
	}else{
		/*insert in bst*/
		new_movie_t *current, *prev = NULL;
		current = new_releases;
		while(current != NULL){
			if(current->movieID == movieID){
				printf("Movie <%d> is already inside the new releases\n", movieID);
				return 0;
			}
			prev = current;
			if(movieID < current->movieID){
				current = current->lc;
			}else{
				current = current->rc;
			}
		}

		if(prev != NULL){
			if(movieID < prev->movieID){
				prev->lc = newMovie;
			}else{
				prev->rc = newMovie;
			}
		}
	}

	printf("New releases Tree:\n");
	if(new_releases == NULL){
		printf("\t<new releases>: <0>\n");
	}else{
		printf("\t<new releases>:");
	}
	
	InOrder(new_releases);

    printf("\n");
	movieOccurrences[category]++; /*counting how many movies in each category*/

	 return 1;
 }



 void InOrderTraversal(new_movie_t *root, int category){
	if(root != NULL){

		InOrderTraversal(root->lc, category);

		if(root->category == category){
			array[indices[category]] = *root;
			indices[category]++;
		}

		InOrderTraversal(root->rc, category);

	}	
 }

 movie_t *arrayToBst(new_movie_t *array, int start, int end){

	if(start > end){
		return NULL;
	}

	int mid = (start + end) / 2;

	movie_t *newNode = (movie_t*)malloc(sizeof(movie_t));
	if(newNode == NULL){
		exit(EXIT_FAILURE);	
	}

	newNode->movieID = array[mid].movieID;
	newNode->year = array[mid].year;
	newNode->sumScore = array[mid].sumScore;
	newNode->watchedCounter = array[mid].watchedCounter;
	newNode->lc = arrayToBst(array, start, mid - 1);
	newNode->rc = arrayToBst(array, mid + 1, end);

	return newNode;

 }

 void printBSTInOrder(movie_t *root) {
    if (root != NULL) {
        printBSTInOrder(root->lc);
        printf(" <%d>,", root->movieID);
        printBSTInOrder(root->rc);
    }
}

void freeNewReleasesTree(new_movie_t *root) {
    if (root != NULL) {
        freeNewReleasesTree(root->lc);
        freeNewReleasesTree(root->rc);
        free(root);
    }
}


 /**
 * @brief Distribute the movies from new release binary tree to the array of categories.
 *
 * @return 0 on success
 *         1 on failure
 */

 int distribute_movies(void){

	int i, j;

	/*initializing the sentinel node*/
	movie_t *sentinel = (movie_t *)malloc(sizeof(movie_t));
	sentinel->movieID = -1;
	sentinel->sumScore = 0;
	sentinel->watchedCounter = 0;
	sentinel->year = 0;
	sentinel->lc = NULL;
	sentinel->rc = NULL;


	for(i = 0; i < 6; i++){

		categoryArray[i] = (movieCategory_t*)malloc(sizeof(movieCategory_t));
		categoryArray[i]->sentinel = sentinel;

		array = (new_movie_t*)malloc(movieOccurrences[i] * sizeof(new_movie_t));
		if(array == NULL){
			return 0;
		}

		InOrderTraversal(new_releases, i); // take the new releases and put each category in an array in ascending order.

		categoryBsts[i] = arrayToBst(array, 0, movieOccurrences[i]-1); // take the array of each category and make a bst that is balanced

		categoryArray[i]->movie = categoryBsts[i]; // and then make the category array point to the root of each bst made


		free(array);	
	}

	print_movies();


	freeNewReleasesTree(new_releases);

	return 1;
 }

 void inOrderLeaves(userMovie_t *root){
	if(root != NULL){
		inOrderLeaves(root->lc);

		if(root->lc == NULL && root->rc == NULL){
			printf("\t<%d, %d>\n", root->movieID, root->score);
		}

		inOrderLeaves(root->rc);
	}
 } 

 void printLeafOriented(userMovie_t *root)
{
	if(root != NULL){
		printLeafOriented(root->lc);
		if(root->category >= 0){
			printf("\t<%d, %d>\n", root->movieID, root->score);
		}
		printLeafOriented(root->rc);
	}
}


 
 /**
 * @brief User rates the movie with identification movieID with score
 *
 * @param userID The identifier of the user
 * @param category The Category of the movie
 * @param movieID The identifier of the movie
 * @param score The score that user rates the movie with id movieID
 *
 * @return 1 on success
 *         0 on failure
 */

 int watch_movie(int userID,int category, int movieID, int score){

	int check, hash_i;
	userMovie_t *insert = NULL, *papa = NULL, *other = NULL;
	user_t *currU = NULL;

	check = search_movie(movieID, category); 
	if(check){
		foundMovie->watchedCounter++;
		foundMovie->sumScore = foundMovie->sumScore + score;

		insert = (userMovie_t*)malloc(sizeof(userMovie_t));
		insert->movieID = movieID;
		insert->category = category;
		insert->score = score;
		insert->parent = NULL;
		insert->lc = NULL;
		insert->rc = NULL;

		hash_i = hashfunction(userID);
		currU = user_hashtable_p[hash_i];

		while(currU != NULL){
			if(currU->userID == userID){

				if(currU->history == NULL){
					currU->history = insert;
					break;
				}else{
					papa = currU->history;
					while(papa != NULL && papa->category < 0){
						if(movieID >= papa->movieID){
							papa = papa->rc;
						}else{
							papa = papa->lc;
						}
					}

					if(papa != NULL){
						other = (userMovie_t *)malloc(sizeof(userMovie_t));
						other->movieID = papa->movieID;
						other->category = papa->category;
						other->score = papa->score;
						other->lc = NULL;
						other->rc = NULL;

						if(insert->movieID > papa->movieID){
							
							insert->parent = papa;

							other->parent = papa;

							papa->movieID = insert->movieID;
							papa->category = -1;
							papa->score = -1;
							papa->rc = insert;
							papa->lc = other;

						}else if(insert->movieID < papa->movieID ){

							insert->parent = papa;
							other->parent = papa;

							papa->category = -1;
							papa->score = -1;
							papa->lc = insert;
							papa->rc = other;

								
						}else{
							papa->score = insert->score;
							printf("Already watched this movie. Only updating the score.\n");

						}
						break;
					}

				}

			}
			currU = currU->next;
		}
		
		if(currU != NULL){
			printf("History tree of user <%d> : \n", currU->userID);
			if(currU->history != NULL){
				printLeafOriented(currU->history);
			}else{
				printf("\tNo history tree SORRY MAN:(\n");
			}
		}else{
			printf("The user <%d> does not exist.\n", userID);
			return 0;
		}
		



	}else{
		printf("The movie does not exist.\n");
		return 0;
	}


	 return 1;
 }

 void inOrderFilter(movie_t *root, int numMovies, int score){
	if(root != NULL){

		inOrderFilter(root->lc, numMovies, score);

		/*conditions*/
		if( (root->sumScore / root->watchedCounter) > score ){
			numMovies++;
		}


		inOrderFilter(root->rc, numMovies, score);
	}
 }

 void inOrderScore(movie_t *root, int score, int i){
	if(root != NULL){
		inOrderScore(root->lc, score, i);

		if( (root->sumScore / root->watchedCounter) > score ){
			scoreArray[i] = *root;
			i++;
		}

		inOrderScore(root->rc, score, i);
	}
 }

 

 userMovie_t *bstMax(userMovie_t *root){
	while(root != NULL && root->rc != NULL){
		root = root->rc;
	}
	return root;
 }

 userMovie_t* findLeftMostLeaf(userMovie_t *root){

	while(root != NULL && root->lc != NULL){
		root = root->lc;
	}
	return root;
 }


 userMovie_t* findNextLeaf(userMovie_t *leaf){
	
	userMovie_t *parent = NULL, *toReturn = NULL;
	if(leaf == NULL){
		return NULL;
	}

	parent = leaf->parent;
	
	if(parent != NULL && parent->rc != NULL && parent->rc != leaf){
		
		toReturn = parent->rc;
		toReturn = findLeftMostLeaf(toReturn);

	}else if(parent != NULL && parent->rc != NULL && parent->rc == leaf){
		
		parent = parent->parent;
		if(parent != NULL && parent->rc != NULL){
			
			toReturn = parent->rc;
			toReturn = findLeftMostLeaf(toReturn);
		}

	}

	return toReturn;


 }

 void EditLeafOriented(userMovie_t *root)
{
	if(root != NULL){
		EditLeafOriented(root->lc);
		if(root->category >= 0){
			scoreSum = scoreSum + root->score;
			counter++;
		}
		EditLeafOriented(root->rc);
	}
}
 
/**
 * @brief Find movies from categories withn median_score >= score t
 *
 * @param userID The identifier of the user
 * @param category Array with the categories to search.
 * @param score The minimum score the movies we want to have
 *
 * @return 1 on success
 *         0 on failure
 */

 int user_stats(int userID){ 

	int hash_i, stats = 0;
	user_t *current = NULL;
	userMovie_t *userMovie= NULL, *check = NULL;

	hash_i = hashfunction(userID);
	current = user_hashtable_p[hash_i];

	while(current != NULL){
		if(current->userID == userID){

			if(current->history != NULL){
				userMovie = current->history;
			}else{
				printf("There is no watch history tree for user <%d>.\n", userID);
				return 0;
			}
			
			EditLeafOriented(userMovie);

			if(counter != 0){
				stats = scoreSum / counter;
			}
			printf("User <%d> statistics: <%d>\n", userID, stats);
			return 1;

		}
		current = current->next;
	}
	printf("The user <%d> does not exist.\n", userID);
	return 0;
 }
 
/**
 * @brief Search for a movie with identification movieID in a specific category.
 *
 * @param movieID The identifier of the movie
 * @param category The category of the movie
 *
 * @return 1 on success
 *         0 on failure
 */
 int search_movie(int movieID, int category){
	movie_t *current = NULL;

	if(categoryArray[category] != NULL){
		current = categoryArray[category]->movie;
		while(current != NULL && current->movieID != -1 && movieID != current->movieID){
			if(movieID < current->movieID){
				current = current->lc;
			}else{
				current = current->rc;
			}
		}

		if( current != NULL && movieID == current->movieID){
			foundMovie = current;
			return 1;
		}

	}

	return 0;

 } 
 
 /**
 * @brief Prints the movies in movies categories array.
 * @return 1 on success
 *         0 on failure
 */

 int print_movies(void){
	int i;
	printf("Movie Category Array:\n");
	for (i = 0; i < 6; i++) {
    	printf("\t<category %d> :", i);
    	printBSTInOrder(categoryArray[i]->movie);
		printf("\n");
	}	
	 return 1;
 }
 
  /**
 * @brief Prints the users hashtable.
 * @return 1 on success
 *         0 on failure
 */

 int print_users(void){

	int i;
	user_t *currU = NULL;
	userMovie_t *userMovie = NULL, *check = NULL;

	for(i = 0; i < hashtable_size; i++){
		printf("Chain <%d> of Users:\n", i);
		currU = user_hashtable_p[i];

		if(currU == NULL){
			printf("\t0\n");
			continue;
		}

		while(currU != NULL){
			printf("\t<%d>\n\tHistory Tree:\n", currU->userID);
			userMovie = currU->history;

			if(userMovie == NULL){
				printf("\t\t0\n");
			}else{
				check = bstMax(userMovie);
				userMovie = findLeftMostLeaf(userMovie);
				while(userMovie != NULL){
					printf("\t\t<%d> <%d>\n", userMovie->movieID, userMovie->score);
					if(userMovie == check){
						break;
					}
					userMovie = findNextLeaf(userMovie);
				}
			}

			currU = currU->next;

		}
	}


	 return 1;
 }
 

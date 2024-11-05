# Movie Managment System
This is a C-based Movie Management and User Interaction System, developed as part of the Winter 2023 project, HY240. It features efficient data structures for managing movies, user data, and interactions between them, including a binary search tree (BST) and a hash table.

## Features

### 1. User Management
- **Register Users**: Add new users and store their data in a hash table for efficient access.
- **Unregister Users**: Remove users and delete their movie history from the system.
- **User History**: Track movies watched and ratings given by each user, stored in a custom binary tree.

### 2. Movie Management
- **Add Movies**: Organize movies into categories and store new releases in a balanced BST.
- **Distribute Movies**: Categorize and distribute movies from the new releases tree into category-specific balanced BSTs.
- **Search**: Search for movies by ID and category.

### 3. User-Movie Interactions
- **Watch and Rate**: Users can watch and rate movies, updating the movie’s overall score and view count.
- **Statistics**: Calculate and display user statistics based on their viewing history.

## Data Structures
- **Binary Search Trees (BSTs)**: Used for storing and organizing movies and user history.
- **Hash Table**: Efficiently manages and looks up user data.
- **Linked Lists**: Used for managing user data within the hash table.

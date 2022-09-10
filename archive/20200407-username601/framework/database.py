from pymongo import MongoClient
from enum import Enum
from time import time

class ModifyType(Enum): # the common types
    CHANGE = "$set"
    REMOVE = "$unset"
    INCREMENT = "$inc"
    APPEND = "$push"
    POP = "$pull"

class Database:
    def __init__(self, database_url: str, database_name: str = "username601"):
        """ Database object. """
        self.db = MongoClient(database_url)[database_name]
        self.types = ModifyType
        self.database_calls = { 'modify': {}, 'eval': 0, 'add': 0, 'delete': 0, 'get': 0 }
        del database_url # for safety purposes lel
    
    def modify(self, collection_name: str, modify_type: ModifyType, query: dict, payload: dict): # resembles a POST request
        """
        Updates/modifies a single part of the database.
        Example:
        db.modify("economy", db.types.CHANGE, {"key_name": "key_value"}, {"key_name_to_change": "new_value"})
        """
        self.database_calls['modify'][modify_type.value] = self.database_calls['modify'].get(modify_type.value, 0) + 1
        self.db[collection_name].update_one(query, {modify_type.value: payload})
        return payload
    
    def eval(self, command: str):
        """ Evaluates stuff """
        self.database_calls['eval'] += 1
        
        try:    return self.db.command(command)
        except: return
    
    def add(self, collection_name: str, new_data: dict): # resembles a PUT request
        """ Adds a document to the database. """
        self.database_calls['add'] += 1
        self.db[collection_name].insert_one(new_data)
        return new_data
    
    def delete(self, collection_name: str, query: dict): # resembles a DELETE request
        """ Removes a document from the database. """
        self.database_calls['delete'] += 1
        self.db[collection_name].delete_one(query)
    
    def get(self, collection_name: str, query: dict) -> dict: # resembles a GET request
        """ Fetches a document from the database. """
        self.database_calls['get'] += 1
        return self.db[collection_name].find_one(query)
    
    def exist(self, collection_name: str, query: dict) -> bool:  # resembles a GET request (sort of)
        """ Checks if a specific query exists. """
        return (self.get(collection_name, query) is not None)
    
    def get_all(self, collection_name: str) -> list: # resembles a hell of a big GET request
        """ Gets every single document from a specific database collection. """
        return self.db[collection_name].find()
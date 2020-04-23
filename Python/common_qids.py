"""
Load all the partial query logs (Wiki, Cweb and Bing) and find the common qids
that are present in all three. Remove those qids from each log that are not
present in the common pool. Sort the logs based on qid, pick top n, print basic
stats, export sampled logs and the common qid list. 
"""

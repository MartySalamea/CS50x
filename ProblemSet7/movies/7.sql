SELECT movies.title, ratings.rating
FROM movies JOIN ratings on ratings.movie_id = movies.id
WHERE movies.year = 2010
ORDER by ratings.rating DESC, movies.title ASC;
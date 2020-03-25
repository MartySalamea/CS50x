SELECT title FROM movies
JOIN stars on stars.movie_id = movies.id
JOIN people on people.id = stars.person_id
JOIN ratings on ratings.movie_id = stars.movie_id
WHERE people.name = "Johnny Depp"
INTERSECT
SELECT title FROM movies
JOIN stars on stars.movie_id = movies.id
JOIN people on people.id = stars.person_id
JOIN ratings on ratings.movie_id = stars.movie_id
WHERE people.name = "Helena Bonham Carter";
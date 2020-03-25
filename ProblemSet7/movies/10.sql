SELECT people.name FROM people
JOIN directors on directors.person_id = people.id
JOIN ratings on ratings.movie_id = directors.movie_id
WHERE ratings.rating >= 9;
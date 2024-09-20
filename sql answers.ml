Q1
SELECT job_id, descr
FROM bookjobs
WHERE cust_id IN (
    SELECT cust_id
    FROM publishers
    WHERE creditcode = 'C'
);



Q2
SELECT item_id, descr
FROM items
WHERE item_id NOT IN (
    SELECT item_id
    FROM po_items
);

Q3
SELECT job_id, po_id
FROM po_items
GROUP BY po_id
HAVING SUM(quantity) > (
    SELECT AVG(total_quantity)
    FROM (
        SELECT po_id, SUM(quantity) AS total_quantity
        FROM po_items
        GROUP BY po_id
    ) AS avg_subquery
);

Q4
SELECT cust_id
FROM bookjobs
WHERE cust_id IN (
    SELECT cust_id
    FROM bookjobs
    WHERE jobtype = 'R'

    UNION

    SELECT cust_id
    FROM bookjobs
    WHERE jobtype = 'H'
    
    EXCEPT

    SELECT cust_id
    FROM bookjobs
    WHERE jobtype = 'N'
);

Q6
SELECT bookjobs.job_id, publishers.cust_id, publishers.name, publishers.city
FROM publishers
INNER JOIN bookjobs ON publishers.cust_id = bookjobs.cust_id;

Q7
SELECT bookjobs.job_id, bookjobs.descr, po_items.po_id, items.item_id, items.descr
FROM po_items
INNER JOIN bookjobs ON po_items.job_id = bookjobs.job_id
INNER JOIN items ON po_items.item_id = items.item_id;

Q8
SELECT bookjobs.job_id, pos.po_id, pos.vendor_id
FROM bookjobs
LEFT JOIN pos ON bookjobs.job_id = pos.job_id;

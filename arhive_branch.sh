#!/bin/bash

echo -n "Enter name of branch to archive: "
read branch
echo "tag archive/$branch $branch"
git tag archive/$branch $branch
echo "push origin archive/$branch"
git push origin archive/$branch
echo "branch -D $branch"
git branch -D $branch
echo "Done!"

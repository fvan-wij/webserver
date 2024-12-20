#!/bin/bash


echo "-------------------------"
git branch
echo "-------------------------"
echo -n "Enter name of branch to archive: "
read branch

set -xe
git tag archive/$branch $branch
git push origin archive/$branch
git branch -D $branch
git push -d origin $branch 

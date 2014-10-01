## Introduction

This Ibus engine is to type Chinese using pinyin
at the difference of other mainstream pinyin engine, this one focus
on forcing the user to type tone in addition to the normal pinyin

it means where in a normal pinyin input you will input

 * nihao
 * nh

to have 你好, here you will have to type

 * ni3hao3
 * n3h3

### Ok, but why?

Mostly for learners, as a learner myself, I found that by typing pinyin
without tones, it was very hard for me when talking orally to remember
which tones to put to words

##Origin note

This is a template engine for IME developers. You could start your IME engine
from it.

Ibus-tmpl was released under GPLV2. We wish ibus engine developers would release
new engines under open source license too, but we do not force it. We allow
developers start your work from ibus-tmpl and swith to other license.

src - engine in c language

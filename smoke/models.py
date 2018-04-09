# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models
from datetime import  datetime
# Create your models here.
class smoke(models.Model):
    uploadtime=models.DateTimeField(auto_now=True)
    value=models.TextField(default='-1')
    longitude=models.TextField(default='0')
    latitude=models.TextField(default='0')
    def __unicode__(self):
       return self.value
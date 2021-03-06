# -*- coding: utf-8 -*-
# Generated by Django 1.11.10 on 2018-03-21 11:16
from __future__ import unicode_literals

import datetime
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('smoke', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='smoke',
            name='latitude',
            field=models.FloatField(default=40),
        ),
        migrations.AlterField(
            model_name='smoke',
            name='longitude',
            field=models.FloatField(default=116),
        ),
        migrations.AlterField(
            model_name='smoke',
            name='uploadtime',
            field=models.DateTimeField(default=datetime.datetime(2018, 3, 21, 19, 16, 9, 560000)),
        ),
        migrations.AlterField(
            model_name='smoke',
            name='value',
            field=models.FloatField(default=-1),
        ),
    ]

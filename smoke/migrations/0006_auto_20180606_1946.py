# -*- coding: utf-8 -*-
# Generated by Django 1.11.10 on 2018-06-06 11:46
from __future__ import unicode_literals

import datetime
from django.db import migrations, models
from django.utils.timezone import utc


class Migration(migrations.Migration):

    dependencies = [
        ('smoke', '0005_smoke_tem'),
    ]

    operations = [
        migrations.AlterField(
            model_name='smoke',
            name='uploadtime',
            field=models.DateTimeField(default=datetime.datetime(2018, 6, 6, 11, 46, 51, 233000, tzinfo=utc)),
        ),
    ]
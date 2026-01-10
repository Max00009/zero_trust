Some of the test_files was not writtable.I checked and turns out i didn't have write permission.to fix:
    To take ownership:
        sudo chown -R $USER:$USER (file_path)
        for example:sudo chown -R max1337:max1337 ./test_files/
    To add write permission:
        chmod u+w (file_path)
    To verify:
        ls -l (file_path)
However i didn't automate this logic inside my main code cause from the user's perspective he will always have permission.my problem is i am sharing test files from one device to another.
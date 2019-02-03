pipeline
{
    agent {
        node {
            label 'ue4 && vs2017'
            customWorkspace 'D:/CI'
        }
    }
    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Generate Project Files') {
            steps {
                bat "BuildScripts/Step1_StartBuild.bat"
            }
        }

        stage('Build Project') {
            steps {
                bat 'nuget restore Superium.sln'
                bat "\"${tool 'MSBuild'}\" Superium.sln /t:build /p:Platform=Win64;verbosity=diagnostic"
            }
        }

        stage('Build Editor') {
            steps {
                bat "BuildScripts/Step2_BuildFiles.bat"
            }
        }

        stage('Build Lighting') {
            steps {
                bat "BuildScripts/Step3_BuildLighting.bat"
            }
        }

        stage('Cook and Deploy') {
            steps {
                bat "BuildScripts/Step4_CookProject.bat"
            }
        }
    }
}
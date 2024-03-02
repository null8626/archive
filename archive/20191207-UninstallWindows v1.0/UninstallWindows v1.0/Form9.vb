Public Class Form9
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Timer1.Start()
        Label1.Text = ("Setting the Time Bomb...")
        Label4.Text = ("Estimated Time Remaining = 1 Second")
        Label3.Text = ("Setting up the Time Bomb for the explosion... Please wait...")
        Label2.Text = ("Please wait for the explosion to begin.")
    End Sub

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        ProgressBar1.Increment(10)
        If ProgressBar1.Value = 100 Then
            ProgressBar1.Value = 100
            Timer1.Stop()
            Button1.Enabled = False
            Label1.Text = ("Time Bomb failed to execute.")
            Label4.Text = ("Estimated Time Remaining = -1 Second")
            Label3.Text = ("ERROR: Time bomb unable to execute.")
            Label2.Text = ("Explosion cannot begin for a problem.")
            MsgBox("ERROR: Time bomb is disabled on this System. Time Bomb is added in Windows 9, and Banned in Windows 10. Please use the OS Windows 9 for this to work.", vbOKOnly + vbInformation, "ERROR: Time bomb already banned/not available in this OS.")
        End If
    End Sub
End Class